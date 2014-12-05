#include "RootIOUtil/InputElementKeeper.h"
#include "RootIOUtil/InputTreeManager.h"
#include "RootIOUtil/InputFileManager.h"
#include "RootIOUtil/SmartRefTable.h"
#include "RootIOUtil/RootFileReader.h"

#include "TTree.h"
#include "TFile.h"

InputElementKeeper* InputElementKeeper::m_keeper = 0;

InputElementKeeper::InputElementKeeper() : m_table(new SmartRefTable), m_treeMgr(new InputTreeManager), m_fileMgr(new InputFileManager), m_refCount(0)
{
  m_keeper = this;
}

InputElementKeeper::~InputElementKeeper()
{
  delete m_table;
  delete m_treeMgr;
  delete m_fileMgr;
  if ( this == m_keeper ) m_keeper = 0;
}

void InputElementKeeper::AddRef()
{
  ++m_refCount;
}

void InputElementKeeper::ClearTable(int fileid)
{
  m_table->DeleteTable(fileid);
}

void InputElementKeeper::DecRef()
{
  if (0 == --m_refCount) delete this;
}

int InputElementKeeper::RegisterFile(std::string& filename, std::vector<JM::TreeMetaData*>& trees)
{
  int fileid = m_fileMgr->FindFile(filename);
  // File already registered.
  if (fileid != -1) return fileid;
  fileid = m_fileMgr->AddFile(filename);
  std::map<int,std::string> treeinfo;
  std::vector<JM::TreeMetaData*>::iterator it;
  for (it = trees.begin(); it != trees.end(); ++it) {
    int treeid = m_treeMgr->AddTree(fileid);
    // Register meta data later when start to read data
    //m_table->ReadMetaData(*it, treeid);
    treeinfo.insert(std::make_pair(treeid, (*it)->GetTreeName()));
  }
  m_fileMgr->SetTreeInfo(fileid, treeinfo);
  return fileid;
}

void InputElementKeeper::RegisterPathMap(const Path2Files& pathmap)
{
  m_path2FileList = pathmap;
}

void InputElementKeeper::AddObjRef(Int_t uid, const TProcessID* pid)
{
  int treeid = m_table->GetTreeID(uid, pid);
  // Unregistered object
  if (-1 == treeid) return;
  m_treeMgr->AddRef(treeid);
}

void InputElementKeeper::AddTreeRef(int fileid)
{
  m_fileMgr->AddTreeRef(fileid);
}

void InputElementKeeper::DecTreeRef(int fileid)
{
  m_fileMgr->DecTreeRef(fileid);
}

void InputElementKeeper::SetNavTreeRef(int fileid)
{
  m_fileMgr->SetNavTreeRef(fileid);
}

void InputElementKeeper::ResetNavTreeRef(int fileid)
{
  m_fileMgr->ResetNavTreeRef(fileid);
}

void InputElementKeeper::DelObj(Int_t uid, TProcessID* pid, Long64_t entry)
{
  int treeid = m_table->GetTreeID(uid, pid, true);
  if (-1 == treeid) return;
  m_treeMgr->DelObj(treeid, entry);
}

InputElementKeeper* InputElementKeeper::GetInputElementKeeper(bool create)
{
  if (!m_keeper && create) m_keeper = new InputElementKeeper;
  return m_keeper;
}

bool InputElementKeeper::CheckFileStatus(int fileid) const
{
  return m_fileMgr->CheckFileStatus(fileid);
}

bool InputElementKeeper::GetNavTree(int fileid, TTree*& tree)
{
  if (!CheckFileStatus(fileid)) {
    OpenFile(fileid); // TODO if fail to open file
  }
  TFile* file = m_fileMgr->GetFile(fileid);
  tree = RootFileReader::GetNavTree(file);
  return true;
}

std::string& InputElementKeeper::GetFileName(int fileid) 
{
  return m_fileMgr->GetFileName(fileid);
}

TFile* InputElementKeeper::GetFile(int fileid)
{
  return m_fileMgr->GetFile(fileid);
}

void InputElementKeeper::OpenFile(int fileid)
{
  std::string filename = m_fileMgr->GetFileName(fileid);
  TFile* file;
  std::map<int,std::string> treeInfo = m_fileMgr->GetTreeInfo(fileid);
  std::vector<TTree*> trees;
  if (!RootFileReader::ReOpen(filename, file, treeInfo, trees)) {
    //TODO failed to open file and get trees
    return;
  }
  m_fileMgr->UpdateFile(fileid, file);

  // Register meta data to SmartRefTable and update pointer to TTree
  m_table->StartNewTable(fileid);

  JM::FileMetaData* fmd = RootFileReader::GetFileMetaData(file);
  std::vector<JM::TreeMetaData*> tmds = fmd->GetTreeMetaData();
  std::map<int,std::string>::iterator it;
  std::vector<TTree*>::iterator it2;
  for(it = treeInfo.begin(), it2 = trees.begin(); it != treeInfo.end(); ++it, ++it2) {
    // Reset the pointer to TTree
    m_treeMgr->ResetTree(it->first, *it2);

    // Put meta data into SmartRefTable when opening file
    std::vector<JM::TreeMetaData*>::iterator tit, tend = tmds.end();
    for (tit = tmds.begin(); tit != tend; ++tit) {
      if (it->second == (*tit)->GetTreeName()) {
        m_table->ReadMetaData(*tit,it->first);
      }
    }
  }
  delete fmd;
}

TBranch* InputElementKeeper::GetBranch(Int_t uid, const TProcessID* pid)
{
  int branchid = m_table->GetBranchID(uid, pid);
  if (-1 == branchid) return 0;
  int treeid = m_table->GetTreeID(uid, pid);
  if (-1 == treeid) return 0;
  TTree* tree = m_treeMgr->GetTree(treeid);
  if (!tree) return 0;
  return (TBranch*)tree->GetListOfBranches()->At(branchid);
}

std::vector<int> InputElementKeeper::GetFileList(const std::string& path)
{
  if (path == "none") {
    std::vector<int> allList;
    Path2Files::iterator it, end = m_path2FileList.end();
    for (it = m_path2FileList.begin(); it != end; ++it) {
      allList.insert(allList.begin(), it->second.begin(), it->second.end());
    }
    return allList;
  }
  Path2Files::iterator pos = m_path2FileList.find(path);
  if (pos == m_path2FileList.end()) {
    return std::vector<int>();
  }
  return pos->second;
}
