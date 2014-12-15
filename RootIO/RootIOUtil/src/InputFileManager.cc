#include "RootIOUtil/InputFileManager.h"
#include "RootIOUtil/InputElementKeeper.h"

#include "TTree.h"
#include "TFile.h"

InputFileHandle::InputFileHandle(const std::string& filename, int fileid) 
        : m_name(filename)
        , m_status(false)
        , m_navTreeRefFlag(false)
        , m_activeTrees(0)
        , m_file(0)
        , m_fileID(fileid)
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
  InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper();
  keeper->ClearTable(m_fileID);
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
  if (m_activeTrees <= 0 && !m_navTreeRefFlag) {
    this->Close();
  }
}

void InputFileHandle::SetNavTreeRef()
{
  m_navTreeRefFlag = true;
}

void InputFileHandle::ResetNavTreeRef()
{
  m_navTreeRefFlag = false;
  if (m_activeTrees <= 0) {
    this->Close();
  }
}

void InputFileHandle::SetTreeInfo(const std::map<int,std::string>& treeinfo) 
{
    m_treeInfo = treeinfo;
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

void InputFileManager::SetTreeInfo(int fileid, const std::map<int,std::string>& treeinfo)
{
  m_files[fileid]->SetTreeInfo(treeinfo);
}

void InputFileManager::AddTreeRef(int fileid)
{
  m_files[fileid]->AddTreeRef();
}

void InputFileManager::CloseFile(int fileid)
{
  m_files[fileid]->Close();
}

void InputFileManager::DecTreeRef(int fileid)
{
  m_files[fileid]->DecTreeRef();
}

void InputFileManager::SetNavTreeRef(int fileid)
{
  m_files[fileid]->SetNavTreeRef();
}

void InputFileManager::ResetNavTreeRef(int fileid)
{
  m_files[fileid]->ResetNavTreeRef();
}

bool InputFileManager::CheckFileStatus(int fileid) const
{
  return m_files[fileid]->GetStatus();
}

void InputFileManager::UpdateFile(int fileid, TFile* file)
{
  m_files[fileid]->UpdateFile(file);
}

std::string& InputFileManager::GetFileName(int fileid)
{
  return m_files[fileid]->GetFileName();
}

TFile* InputFileManager::GetFile(int fileid)
{
  return m_files[fileid]->GetFile();
}

std::map<int,std::string>& InputFileManager::GetTreeInfo(int fileid)
{
  return m_files[fileid]->GetTreeInfo();
}
