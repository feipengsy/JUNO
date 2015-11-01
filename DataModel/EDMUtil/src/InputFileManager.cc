#include "InputFileManager.h"
#include "InputElementKeeper.h"

#include "TTree.h"
#include "TFile.h"

InputFileHandle::InputFileHandle(const std::string& filename, int fileid) 
        : m_name(filename)
        , m_status(false)
        , m_activeTrees(0)
        , m_activeEntries(0)
        , m_fileID(fileid)
        , m_file(0)
{
}

InputFileHandle::~InputFileHandle()
{
    // destructor
    if (m_status) {
        // close the file first
        m_file->Close();
    }
    delete m_file;
}

void InputFileHandle::Close()
{
  m_file->Close();
  m_status = false;
}

void InputFileHandle::UpdateFile(TFile* file) 
{
  m_file = file;
  m_status = true;
}

void InputFileHandle::AddTreeRef()
{
  ++m_activeTrees;
}

void InputFileHandle::DecTreeRef()
{
  --m_activeTrees;
  this->CheckClose();
}

void InputFileHandle::AddObjRef()
{
  ++m_activeEntries;
}

void InputFileHandle::DecObjRef()
{
  --m_activeEntries;
  this->CheckClose();
}

void InputFileHandle::AddTreeInfo(int treeid, const std::string& treename) 
{
    m_treeInfo.insert(std::make_pair(treeid, treename));
}

void InputFileHandle::CheckClose()
{
  if (m_activeTrees == 0 && m_activeEntries ==0) {
    this->Close();
    InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper();
    keeper->ClearTable(m_fileID);
  }

}

InputFileManager::~InputFileManager()
{
    // destructor
    std::vector<InputFileHandle*>::iterator it, itend = m_files.end();
    // delete all the InputFileHandler
    for (it = m_files.begin(); it != itend; ++it) {
        delete *it;
    }
}

int InputFileManager::AddFile(const std::string& filename)
{
  int fileid = m_files.size();
  m_files.push_back(new InputFileHandle(filename, fileid));
  return fileid;
}

int InputFileManager::FindFile(const std::string& filename)
{
  std::vector<InputFileHandle*>::iterator it, itend = m_files.end();
  for (it = m_files.begin(); it != itend; ++it) {
    if ((*it)->GetFileName() == filename) return it - m_files.begin();
  }
  return -1;
}

void InputFileManager::AddTreeInfo(int fileid, int treeid, const std::string& treename)
{
  m_files[fileid]->AddTreeInfo(treeid, treename);
}

void InputFileManager::AddTreeRef(int fileid)
{
  m_files[fileid]->AddTreeRef();
}

void InputFileManager::DecTreeRef(int fileid)
{
  m_files[fileid]->DecTreeRef();
}

void InputFileManager::AddObjRef(int fileid)
{
  m_files[fileid]->AddObjRef();
}

void InputFileManager::DecObjRef(int fileid)
{
  m_files[fileid]->DecObjRef();
}

bool InputFileManager::CheckFileStatus(int fileid) const
{
  return m_files[fileid]->GetStatus();
}

void InputFileManager::UpdateFile(int fileid, TFile* file)
{
  m_files[fileid]->UpdateFile(file);
}

const std::string& InputFileManager::GetFileName(int fileid)
{
  return m_files[fileid]->GetFileName();
}

TFile* InputFileManager::GetFile(int fileid)
{
  return m_files[fileid]->GetFile();
}

const std::map<int,std::string>& InputFileManager::GetTreeInfo(int fileid)
{
  return m_files[fileid]->GetTreeInfo();
}
