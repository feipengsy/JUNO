#include "RootIOUtil/InputTreeManager.h"
#include "RootIOUtil/InputElementKeeper.h"

#include "TTree.h"

void InputTreeHandle::SetTree(TTree* tree)
{
    m_tree = tree;
    m_active = true;
}

bool InputTreeHandle::LastObj(Int_t uid)
{
    if (!m_active) return false;
    if (uid == m_lastUID) {
        return true;
    }
    return false;
}

InputTreeManager::~InputTreeManager()
{
    // Destructor
    std::vector<InputTreeHandle*>::iterator it, itend = m_trees.end();
    for (it = m_trees.begin(); it != itend; ++it) {
        delete *it;
    }
}

int InputTreeManager::AddTree(int fileid, const std::vector<Long64_t>& breakPoints)
{
  InputTreeHandle* tree = new InputTreeHandle(fileid);
  tree->SetBreakPoints(breakPoints);
  m_trees.push_back(tree);
  return m_trees.size() - 1;
}

void InputTreeManager::AddRef(int treeid)
{
  int active = m_trees[treeid]->AddRef();
  if (1 == active) {
    InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper();
    int fileid = m_trees[treeid]->GetFileID();
    keeper->AddTreeRef(fileid);
  }
}

void InputTreeManager::DelObj(int treeid, Int_t uid)
{
  bool fdel = m_trees[treeid]->LastObj(uid);
  if (fdel) {
    m_trees[treeid]->Close();
    InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper();
    int fileid = m_trees[treeid]->GetFileID();
    keeper->DecTreeRef(fileid);
  }
}

TTree* InputTreeManager::GetTree(int treeid)
{
  InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper();
  int fileid = m_trees[treeid]->GetFileID();
  if (!keeper->CheckFileStatus(fileid)) {
    keeper->OpenFile(fileid);
  }
  return m_trees[treeid]->GetTree();
}

void InputTreeManager::ResetTree(int treeid, TTree* tree)
{
  m_trees[treeid]->SetTree(tree);
}

void InputTreeManager::SetLastUID(int treeid, Int_t uid)
{
  m_trees[treeid]->SetLastUID(uid);
}

int InputTreeManager::GetFileID(int treeid)
{
  return m_trees[treeid]->GetFileID();
}

Long64_t InputTreeManager::GetTreeOffset(int treeid, int offsetid)
{
  return m_trees[treeid]->GetTreeOffset(offsetid);
}

const std::vector<Long64_t>& InputTreeManager::GetBreakPoints(int treeid)
{
  return m_trees[treeid]->GetBreakPoints();
}
