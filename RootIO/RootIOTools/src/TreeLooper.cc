#include "TreeLooper.h"

#include "TTree.h"
#include "Event/EventObject.h"

//------------------- TreeMerger ---------------------

TreeMerger::TreeMerger(const std::string& path, const std::string& objName)
       : m_oTree(0)
       , m_iTree(0)
       , m_addr(0)
       , m_idx(0)
       , m_entries(0)
{
    std::string title = "Tree at " + path + " holding " + objName;
    std::string treeName = objName.substr(objName.rfind("::")+2);
    std::string branchName = treeName;
    m_oTree = new TTree(treeName.c_str(), title.c_str());
    m_oTree->Branch(branchName.c_str(), objName.c_str(), &m_addr,16000,1);
}

TreeMerger::~TreeMerger()
{
    //Yes, we do NOT delete TTree*
}

bool TreeMerger::next()
{
    if (!m_iTree || m_idx >= m_entries) {
        return false;
    }
    m_iTree->GetEntry(m_idx++);
    m_oTree->Fill();
    delete static_cast<JM::EventObject*>(m_addr);
    m_addr = 0;
    return true;
}

void TreeMerger::newTree(TTree* tree)
{
    m_idx = 0;
    m_iTree = tree;
    static_cast<TBranch*>(m_iTree->GetListOfBranches()->At(0))->SetAddress(m_addr);
    m_entries = m_iTree->GetEntries();
}


//------------------- TreeLooper ---------------------

TreeLooper::TreeLooper(const PathMap& dataPathMap)
{
}

TreeLooper::~TreeLooper()
{
}

void TreeLooper::initialize()
{

}

bool TreeLooper::next()
{
    return true;
}

void TreeLooper::finalize()
{

}

void TreeLooper::newInputFile(const std::string& value)
{
    //Set break point of previous file
    
    //Read trees
}
