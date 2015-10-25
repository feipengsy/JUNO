#include "RootIOUtil/RootFileReader.h"
#include "RootIOUtil/InputElementKeeper.h"
#include "RootIOUtil/NavTreeList.h"
#include "RootIOUtil/TreeMetaData.h"
#include "RootIOUtil/FileMetaData.h"
#include "RootIOUtil/UniqueIDTable.h"
#include "SniperKernel/SniperLog.h"

#include "TTree.h"
#include "TFile.h"

#include <algorithm>
#include <iostream>

using namespace std;

JM::FileMetaData* RootFileReader::GetFileMetaData(TFile* file)
{
  TObject* obj = file->Get("Meta/FileMetaData");
  if (!obj) return 0;
  return dynamic_cast<JM::FileMetaData*>(obj);
}

JM::UniqueIDTable* RootFileReader::GetUniqueIDTable(TFile* file)
{
  TObject* obj = file->Get("Meta/UniqueIDTable");
  if (!obj) return 0;
  return dynamic_cast<JM::UniqueIDTable*>(obj);
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

TTree* RootFileReader::GetNavTree(TFile* file)
{
  return static_cast<TTree*>(file->Get("/Meta/navigator"));
}

TTree* RootFileReader::GetDataTree(TFile* file, const std::string& treename) 
{
  return static_cast<TTree*>(file->Get(treename.c_str()));
}

static TObject* ReadObject(const std::string& filename, const std::string& objName)
{
  TObject* obj = 0;
  TFile* file = OpenFile(filename);
  if (!file) return 0;
  return file->Get(objName.c_str());
}

TObject* RootFileReader::ReadObject(TDirectory* dir, const std::string& name)
{
  TObject* obj = 0;
  obj = dir->Get(name.c_str());
  return obj;
}
