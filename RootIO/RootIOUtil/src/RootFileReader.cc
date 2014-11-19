#include "RootIOUtil/RootFileReader.h"
#include "RootIOUtil/InputElementKeeper.h"
#include "RootIOUtil/NavTreeList.h"
#include "SniperKernel/SniperLog.h"

#include <algorithm>

using namespace std;

bool RootFileReader::AddFile(const string& filename)
{
    vector<string>::iterator it = find(m_filenames.begin(), m_filenames.end(), filename);
    // Reduplicated file found, which should be a mistake made by user
    if (!(it == m_filenames.end())) return false;
    m_filenames.push_back(filename);
    return true;
}

bool RootFileReader::ReOpen(const string& filename, TFile*& file, const map<int,string>& treeinfo, vector<TTree*>& trees)
{
    file = OpenFile(filename);
    if (!file) {
        //LogError << "Fail to open file: " << filename
        //         << endl;
        return false;
    }
    // Get the registered trees again
    TObject* obj = 0;
    map<int,string>::const_iterator it;
    for (it = treeinfo.begin(); it != treeinfo.end(); ++it) {
      obj = file->Get(it->second.c_str());
      if (!obj) {
          //LogError << "Failed to get TTree: " << it->second 
          //         << " from file: " << filename
          //         << endl;
          return false;
      }
      TTree* tree = dynamic_cast<TTree*>(obj);
      if (!tree) {
          //LogError << "Failed to get TTree: " << it->second 
          //         << " from file: " << filename
          //         << endl;
          return false;
      }
      trees.push_back(tree);
    }
    // Register the meta data of this file into SmartRefTable
    return true;
}

bool RootFileReader::ReadFiles(NavTreeList* navs, vector<string>& path, vector<string>& eventName)
{
  vector<string>::iterator it;
  map<int,JM::FileMetaData*> fileMetaDatas;
  map<string, vector<int> > path2FileList;
  for (it = m_filenames.begin(); it != m_filenames.end(); ++it) {
    TFile* f = OpenFile(*it);
    if (!f) { 
        LogError << "Fail to open file: " << *it
                 << endl;
        return false;
    }
    JM::FileMetaData* fmetadata = GetFileMetaData(f);
    if (!fmetadata) {
        LogError << "Fail to read file metadata for file: " << *it
                 << endl;
        return false;
    }

    // Get tree metadatas in file metadata, and register this file in InputElementKeeper
    vector<JM::TreeMetaData*> tmetadatas = fmetadata->GetTreeMetaData();
    // keeper should already be initialized by RootInputSvc
    int fileid = m_keeper->RegisterFile(*it, tmetadatas);
    
    fileMetaDatas.insert(make_pair(fileid, fmetadata));

    // Construct the path2files map
    vector<JM::TreeMetaData*>::iterator tit, tend = tmetadatas.end();
    for (tit = tmetadatas.begin();tit != tend;++tit) {
        map<string, vector<int> >::iterator pathpos = path2FileList.find((*tit)->GetTreeName());
        if (pathpos == path2FileList.end()) {
            // miss
            path2FileList.insert(make_pair((*tit)->GetTreeName(), vector<int>(1,fileid)));
        }
        else {
            // hit
            pathpos->second.push_back(fileid);
        }
    }
    f->Close();
  }
  m_keeper->RegisterPathMap(path2FileList);

  // coffee time over, now get down to business

  if (!GetNavTreeList(fileMetaDatas, navs, path, eventName)) return false; //TODO fail to get NavTreeList
  
  // Cleaning gabage
  map<int,JM::FileMetaData*>::iterator fit,fitend = fileMetaDatas.end();
  for (fit = fileMetaDatas.begin(); fit != fitend; ++fit) {
      delete fit->second;
  }
  return true;
}

JM::FileMetaData* RootFileReader::GetFileMetaData(TFile* file)
{
  TObject* obj = file->Get("FileMetaData");
  if (!obj) return 0;
  return dynamic_cast<JM::FileMetaData*>(obj);
}

TFile* RootFileReader::OpenFile(const string& filename)
{
  TFile* f = TFile::Open(filename.c_str(), "READ");
  if (!f) {
    return 0;
  }
  if(!f->IsOpen()) {
    return 0;
  }
  return f;
}

bool RootFileReader::GetNavTreeList(map<int,JM::FileMetaData*>& fmetadatas, NavTreeList* navs, vector<string>& path, vector<string>& eventName)
{
  // Construct NavTreeList for InputStream, using prority of navigators
  // TODO Add sanity check
  
  map<int,int> fid2priority;
  int highest = 0;
  for (map<int,JM::FileMetaData*>::iterator it = fmetadatas.begin(); it != fmetadatas.end(); ++it) {
    int priority = it->second->GetNavPriority();
    highest = highest>priority ? highest:priority;
    fid2priority.insert(make_pair(it->first, priority));
  }
  for (map<int,int>::iterator it = fid2priority.begin(); it != fid2priority.end(); ++it) {
    if (it->second == highest) {
      int fileid = it->first;
      NavTreeHandle* tree = new NavTreeHandle(fileid,  m_keeper->GetFileName(fileid));
      navs->push_back(tree);
    }
  }
  path = fmetadatas[fid2priority.begin()->first]->GetNavPath();
  eventName = fmetadatas[fid2priority.begin()->first]->GetNavEventName();
  return true;
}

TTree* RootFileReader::GetNavTree(TFile* file)
{
  // TODO Define the name of navigator tree
  return dynamic_cast<TTree*>(file->Get("/Event/navigator"));
}

TObject* RootFileReader::GetUserData(const std::vector<int>& fileList, const std::string& name)
{
  TObject* obj = 0;
  std::vector<int>::const_iterator it, end = fileList.end();
  for (it = fileList.begin();it != end; ++it) {
    bool preStatus = m_keeper->CheckFileStatus(*it);
    TFile* file;
    if (preStatus) {
      // File not closed
      file = m_keeper->GetFile(*it);
      obj = ReadObject(file,name);
    }
    else {
      file = OpenFile(m_keeper->GetFileName(*it));
      obj = ReadObject(file,name);
      file->Close();
    }
    if (obj) return obj;
  }
  return obj;
}

TObject* RootFileReader::ReadObject(TDirectory* dir, const std::string& name)
{
  TObject* obj = 0;
  obj = dir->Get(name.c_str());
  if (!obj) {
    // Just in case user forgets the path
    // TODO hard code
    std::string geoname = std::string("Geometry/") + name;
    obj = dir->Get(geoname.c_str());
  }
  return obj;
}
