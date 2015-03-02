#include "TreeLooper.h"

#include "TTree.h"

//------------------- TreeMerger ---------------------

TreeMerger::TreeMerger(const std::string path, const std::string headerName, const std::string eventName)
       : m_oHeader(0)
       , m_oEvent(0)
       , m_iHeader(0)
       , m_iEvent(0)
       , m_hAddr(0)
       , m_eAddr(0)
       , mhIdx(0) 
       , m_eIdx(0)
{
    m_oHeader = new TTree();
    m_oHeader->Branch();
    m_oEvent = new TTree();
    m_oEvent->Branch();
}

TreeMerger::~TreeMerger()
{
    //Yes, we do NOT delete TTree*
}

bool TreeMerger::next()
{
}

void TreeMerger::newTree(TTree* header, TTree* event)
{
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
