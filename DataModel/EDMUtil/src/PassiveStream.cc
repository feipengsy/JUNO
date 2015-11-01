#include "InputElementKeeper.h"
#include "PassiveStream.h"
#include "TTree.h"

PassiveStream::PassiveStream()
  : m_lastReadTreeIndex(-1)
{
}

PassiveStream::~PassiveStream()
{
    for (std::vector<TreeHandle*>::iterator it = m_trees.begin(); it != m_trees.end(); ++it) {
        delete *it;
    }
}

PassiveStream::TreeHandle::TreeHandle(int fileid)
  : fileID(fileid)
  , tree(0)
{
}

int PassiveStream::AddTree(int fileid)
{
    m_trees.push_back(new TreeHandle(fileid));
    return m_trees.size() - 1;
}

void PassiveStream::UpdateTree(int treeIndex, TTree* tree)
{
    m_trees[treeIndex]->tree = tree;
}

bool PassiveStream::ReadObject(int treeIndex, Long64_t entry)
{
    if (-1 != m_lastReadTreeIndex && treeIndex != m_lastReadTreeIndex) {
        // Jumping from one tree to another, means that one file is done read
        // And another is started
        InputElementKeeper::GetInputElementKeeper()->DecTreeRef(m_trees[m_lastReadTreeIndex]->fileID);
        m_lastReadTreeIndex = treeIndex;
        InputElementKeeper::GetInputElementKeeper()->AddTreeRef(m_trees[treeIndex]->fileID);
    }
    TBranch* branch = (TBranch*)m_trees[treeIndex]->tree->GetListOfBranches()->At(0);
    if (!branch) return false;
    void* addr = 0;
    branch->SetAddress(&addr);
    return branch->GetEntry(entry);
}
