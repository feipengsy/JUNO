#include "TreeLooper.h"

#include "RootIOUtil/RootFileReader.h"
#include "RootIOUtil/FileMetaData.h"
#include "RootIOUtil/TreeMetaData.h"
#include "TObject.h"
#include "TTree.h"
#include "TFile.h"

//------------------- TreeMerger ---------------------

TreeMerger::TreeMerger(const std::string& path, const std::string& objName)
       : m_oTree(0), m_iTree(0), m_addr(0), m_idx(0), m_entries(0)
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
    delete static_cast<TObject*>(m_addr);
    m_addr = 0;
    return true;
}

void TreeMerger::newTree(TTree* tree, const std::vector<int>& iBreakPoints)
{
    if (m_oTree->GetEntries()) {
        m_breakPoints.push_back(m_oTree->GetEntries());
    }
    if (m_breakPoints.size()) {
        // Link the break points
        std::vector<int>::const_iterator it, end = iBreakPoints.end();
        for (it = iBreakPoints.begin(); it != end; ++it) {
            m_breakPoints.push_back(*it + m_oTree->GetEntries());
        }
    }
    else {
        // Replace the break points directly
        m_breakPoints = iBreakPoints;
    }
    m_idx = 0;
    m_iTree = tree;
    static_cast<TBranch*>(m_iTree->GetListOfBranches()->At(0))->SetAddress(m_addr);
    m_entries = m_iTree->GetEntries();
}

void TreeMerger::writeTree()
{
    m_oTree->Write(NULL,TObject::kOverwrite);
}


//------------------- TreeLooper ---------------------

TreeLooper::TreeLooper(const PathMap& dataPathMap, TFile* file)
      : m_outputFile(file), m_inputFile(0), m_iNavTree(0), m_oNavTree(0), m_addr(0), m_idx(0), m_entries(0)
{
    // Build data tree map
    PathMap::const_iterator it, end = dataPathMap.end();
    for (it = dataPathMap.begin(); it != end; ++it) {
        m_treeMap.insert(std::make_pair<std::string, TreeMerger*>(it->first, new TreeMerger(it->first, it->second)));
    }
    // Build nav tree
    m_oNavTree = new TTree("navigator", "Tree for EvtNavigator");
    m_oNavTree->Branch("EvtNavigator", "JM::EvtNavigator", &m_addr);
}

TreeLooper::~TreeLooper()
{
    TreeMap::iterator it, end = m_treeMap.end();
    for (it = m_treeMap.begin(); it != end; ++it) {
        delete it->second;
    }
}

bool TreeLooper::next()
{
    bool notYetDone = false;
    // Fill nav tree
    if (m_iNavTree && m_idx < m_entries) {
        notYetDone = true;
        m_iNavTree->GetEntry(m_idx++);
        m_oNavTree->Fill();
        delete static_cast<TObject*>(m_addr);
        m_addr = 0;
    }
    // Fill data trees
    TreeMap::iterator it, end = m_treeMap.end();
    for (it = m_treeMap.begin(); it != end; ++it) {
        notYetDone = it->second->next();
    }
    return notYetDone;
}

void TreeLooper::finalize()
{
    // Write nav tree
    m_outputFile->cd();
    if (!gDirectory->cd("Meta")) {
        gDirectory->mkdir("Meta");
        gDirectory->cd("Meta");
    }
    m_oNavTree->Write(NULL,TObject::kOverwrite);
    // Write data trees
    TreeMap::iterator it, end = m_treeMap.end();
    for (it = m_treeMap.begin(); it != end; ++it) {
        m_outputFile->cd();
        std::string path = (it->first[0] == '/' ? it->first.substr(1) : it->first);
        if (!gDirectory->cd(path.c_str())) {
            gDirectory->mkdir(path.c_str());
            gDirectory->cd(path.c_str());
        }
        it->second->writeTree();
        m_breakPoints[it->first] = it->second->getBreakPoints();
    }
}

void TreeLooper::newInputFile(const std::string& value)
{
    //Read trees
    if (m_inputFile) {
        m_inputFile->Close();
        delete m_inputFile;
        m_inputFile = 0;
    }
    m_inputFile = new TFile(value.c_str(), "read");
    // Get nav tree
    m_iNavTree = RootFileReader::GetNavTree(m_inputFile);
    static_cast<TBranch*>(m_iNavTree->GetListOfBranches()->At(0))->SetAddress(&m_addr);
    m_entries = m_iNavTree->GetEntries();
    m_idx = 0;
    // Get break points from FileMetaData
    JM::FileMetaData* fmd = RootFileReader::GetFileMetaData(m_inputFile);
    BreakPointsMap& breakPoints = fmd->GetBreakPoints();
    // Get data trees
    TreeMap::iterator it, end = m_treeMap.end();
    for (it = m_treeMap.begin(); it != end; ++it) {
        TTree* dataTree = RootFileReader::GetDataTree(m_inputFile, it->first);
        it->second->newTree(dataTree, breakPoints[it->first]);
    }
}
