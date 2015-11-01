#include "RootFileInter.h"
#include "InputElementKeeper.h"
#include "FileMetaData.h"
#include "UniqueIDTable.h"

#include "TTree.h"
#include "TFile.h"

using namespace std;

JM::FileMetaData* RootFileInter::GetFileMetaData(TFile* file)
{
  TObject* obj = file->Get("Meta/FileMetaData");
  if (!obj) return 0;
  return dynamic_cast<JM::FileMetaData*>(obj);
}

JM::UniqueIDTable* RootFileInter::GetUniqueIDTable(TFile* file)
{
  TObject* obj = file->Get("Meta/UniqueIDTable");
  if (!obj) return 0;
  return dynamic_cast<JM::UniqueIDTable*>(obj);
}

TFile* RootFileInter::OpenFile(const string& filename)
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

TTree* RootFileInter::GetTree(TFile* file, const std::string& treename) 
{
  return static_cast<TTree*>(ReadObject(file, treename));
}

TObject* RootFileInter::ReadObject(const std::string& filename, const std::string& objName)
{
  TFile* file = OpenFile(filename);
  if (!file) return 0;
  return file->Get(objName.c_str());
}

TObject* RootFileInter::ReadObject(TFile* file, const std::string& objName)
{
  if (!file) return 0;
  return file->Get(objName.c_str());
}
