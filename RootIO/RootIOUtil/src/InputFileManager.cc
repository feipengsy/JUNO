#include "RootIOUtil/InputFileManager.h"
#include "RootIOUtil/InputElementKeeper.h"

#include "TTree.h"
#include "TFile.h"

InputFileHandle::InputFileHandle(std::string filename) : m_name(filename), m_status(false), m_activeTrees(0), m_file(0)
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

void InputFileHandle::close()
{
  m_file->Close();
  m_status = false;
}

void InputFileHandle::UpdateFile(TFile* file) 
{
  m_file = file;
  m_status = true;
}

void InputFileHandle::SetTreeInfo(std::map<int,std::string>& treeinfo) 
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

int InputFileManager::AddFile(std::string& filename)
{
  m_files.push_back(new InputFileHandle(filename));
  return m_files.size() - 1;
}

int InputFileManager::FindFile(std::string& filename)
{
  std::vector<InputFileHandle*>::iterator it, itend = m_files.end();
  for (it = m_files.begin(); it != itend; ++it) {
    if ((*it)->GetFileName() == filename) return it - m_files.begin();
  }
  return -1;
}

void InputFileManager::SetTreeInfo(int fileid, std::map<int,std::string>& treeinfo)
{
  m_files[fileid]->SetTreeInfo(treeinfo);
}

void InputFileManager::AddTreeRef(int fileid)
{
  m_files[fileid]->AddTreeRef();
}

void InputFileManager::DecTreeRef(int fileid)
{
  int active = m_files[fileid]->DecTreeRef();
  if (0 == active) close(fileid);
}

bool InputFileManager::CheckFileStatus(int fileid) const
{
  return m_files[fileid]->GetStatus();
}

void InputFileManager::close(int fileid)
{
  m_files[fileid]->close();
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
