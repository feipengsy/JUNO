#include "RootIOSvc/RootFileWriter.h"
#include "SniperKernel/SniperLog.h"
#include "EvtNavigator/EvtNavigator.h"
#include "Event/HeaderObject.h"
#include "RootIOUtil/TreeMetaData.h"
#include "RootIOUtil/RootOutputFileManager.h"

#include "TObject.h"
#include "TTree.h"
#include "TFile.h"
#include "TProcessID.h"
#include "TDirectory.h"

RootFileWriter::RootFileWriter(const std::string& path, const std::string& headerName)
    : m_file(0)
    , m_headerTree(0)
    , m_navTree(0)
    , m_treeMetaData(0)
    , m_dir(0)
    , m_path(path)
    , m_headerName(headerName)
    , m_withNav(false)
    , m_initialized(false)
    , m_fileEntries(0)
    , m_navAddr(0)
{
    
}

RootFileWriter::~RootFileWriter()
{
    this->close();
}

RootOutputFileHandle* RootFileWriter::getFile()
{
    return m_file;
}

bool RootFileWriter::write()
{
    // Try to initialize if nessassry
    if (!m_initialized) {
        this->initialize();
    }
    // Check rationality
    if (!m_file) {
        LogError << "No output file started, can not write"
                 << std::endl;
        return false;
    }
    if (m_withNav && !m_navTree) {
        LogError << "No tree for EvtNavigator, can not write"
                 << std::endl;
        return false;
    }
    if (!m_navAddr) {
        LogError << "Address not set, can not write"
                 << std::endl;
        return false;
    }

    bool write = static_cast<JM::EvtNavigator*>(m_navAddr)->writePath(m_path);
    if (!m_initialized || !write) {
        // Currently unknown stream or skipped entry, just idling
        ++m_fileEntries;
        if (m_withNav) {
            this->writeNav();
        }
        return true;  
    }

    // TODO Build lazy-loading data for UniqueIDTable

    bool ok = this->writeEvent();
    if (!ok) {
        LogError << "Fail to write event data for " << m_path
                 << std::endl;
        return false;
    }

    ok = this->writeHeader();
    if (!ok) {
        LogError << "Fail to write header data for " << m_path
                 << std::endl;
        return false;
    }


    if (m_withNav) ok = this->writeNav();
    if (!ok) {
        LogError << "Fail to write EvtNavigator with " << m_path
                 << std::endl;
        return false;
    }

    ++m_fileEntries;
    this->resetAddress();    

    return true;
}

bool RootFileWriter::writeHeader()
{
    int nbytes = m_headerTree->Fill();
    LogDebug <<  "Wrote " << nbytes
             << " bytes to entry " << m_entries
             << " of  header of " << m_path
             << std::endl;

    return nbytes > 0;
}

bool RootFileWriter::writeEvent()
{
    return true;
}

bool RootFileWriter::writeNav()
{
    if (0 == m_fileEntries) {
        // Tell FileMetaData what paths EvtNavigator privides.
        std::vector<std::string> eventNames, paths = static_cast<JM::EvtNavigator*>(m_navAddr)->getPath();
        std::vector<std::string>::iterator it, end = paths.end();
        m_file->setNavPath(paths);
    }
    m_file->setNavAddr(m_navAddr);
    int nbytes = m_navTree->Fill();
    LogDebug <<  "Wrote " << nbytes
             << " bytes to entry " << m_entries
             << " of tree for EvtNavigator "
             << std::endl;

    return nbytes > 0;
}

void RootFileWriter::fillBID(TObject* obj, int bid) 
{
    UInt_t uid = obj->GetUniqueID();
    TProcessID* pid = TProcessID::GetProcessWithUID(uid,obj);
    const char* guid = pid->GetTitle();
    int iid;
    GUIDVector::const_iterator posPID = find( m_guid.begin(), m_guid.end(), guid);
    if (posPID == m_guid.end()) {
        m_guid.push_back(guid);
        m_uid.push_back(std::vector<Int_t>());
        // When bid is -1, branch id won't be saved
        if (-1 != bid) {
            m_bid.push_back(std::vector<Short_t>());
        }
        iid = m_guid.size() - 1;
    }
    else {
        iid = posPID - m_guid.begin();
    }
    uid = uid & 0xffffff;
    m_uid[iid].push_back(uid);
    if (-1 != bid) {
        m_bid[iid].push_back(bid);
    }
}

bool RootFileWriter::close()
{
    // Reset current dir
    m_dir->cd();
    // Write trees
    m_tree->Write(NULL,TObject::kOverwrite);
    // Tree for EvtNavigator will be written by file handle
    //if (m_withNav) m_navTree->Write(NULL,TObject::kOverwrite);
    // Set TreeMetaData
    m_file->addTreeMetaData(m_treeMetaData);
    m_file->addUniqueIDTable(m_path, m_guid, m_uid, m_bid);
    m_file->addUUID(m_guid);
    // Dec file reference, close when it hits 0
    RootOutputFileManager::get()->close_file(m_file->getName());
    // Reset pointers
    m_file = 0;
    m_tree = 0;
    m_navTree = 0;
    m_dir = 0;
    m_withNav = false;
    // Reset file entry
    m_fileEntries = 0;
    // Clear and reclaim memory
    m_guid = GUIDVector();
    m_uid = UIDVector();
    m_bid = BIDVector();
    return true;
}

void RootFileWriter::revise()
{
    // Does EvtNavigator still need to be written with this stream?
    this->checkFilePath();
}

void RootFileWriter::newFile(RootOutputFileHandle* file)
{
    m_file = file;
}

void RootFileWriter::initialize()
{
    if ("unknown" == m_headerName) {
        // Still unknown stream, can not initialize
        return;
    }
    // Make the directories up to but not including last one which is
    // the tree name.
    TFile* rootFile =  m_file->getFile();
    m_dir = rootFile;
    std::string::size_type last = 0, slash = m_path.find('/');
    for (; slash != std::string::npos; slash = m_path.find('/',last)) {
        if (!slash) {
            last = slash + 1;
            continue;   // skip initial '/'
        }
        std::string subdir = m_path.substr(last,slash-last);
        TDirectory* dir = m_dir->GetDirectory(subdir.c_str());
        if (dir) m_dir = dir;
        else m_dir = m_dir->mkdir(subdir.c_str());
        last = slash + 1;
    }
    if (last) {
        std::string subdir = m_path.substr(0,last-1);
        rootFile->cd(subdir.c_str());
    }

    // Make the TTree holding data
    std::string title = "Tree at " + m_path + " holding " + m_eventName;
    std::string treename = m_path.substr(last).substr(m_path.substr(last).rfind("::")+1);

    std::string branchname_header = m_headerName.substr(m_headerName.rfind("::")+2);
    std::string branchname_event = m_eventName.substr(m_eventName.rfind("::")+2);

    m_tree = new TTree(treename.c_str(),title.c_str());
    m_tree->Branch(branchname_header.c_str(),m_headerName.c_str(),&m_headerAddr,16000,1);
    // TODO if it is nessesary for users to decide the buffsize and split level
    // Set split to 1 by default
    m_tree->Branch(branchname_event.c_str(),m_eventName.c_str(),&m_eventAddr,16000,1);

    m_treeMetaData = new JM::TreeMetaData();
    m_treeMetaData->SetTreeName(m_path.substr(0,last)+treename);

    m_file->occupyPath(m_path);
    this->checkFilePath();
    m_initialized = true;
}

void RootFileWriter::setAddress(JM::EvtNavigator* nav)
{
    m_navAddr = nav;
    JM::HeaderObject* header = nav->getHeader(m_path);
    m_headerTree->setAddress(header);
    String2TreeHandle::iterator it, end = m_eventTrees.end();
    for (it = m_eventTrees.begin(); it != end; ++it) {
        it->second->setAddress(header->event(it->first));
    }
}

void RootFileWriter::setHeaderName(const std::string& name)
{
    m_headerName = name;
}

int RootFileWriter::entries()
{
    return m_entries;
}

int RootFileWriter::fileEntries()
{
    return m_fileEntries;
}

void RootFileWriter::checkFilePath()
{
    // Make the TTree holding EvtNavigator
    // Save EvtNavigator together with the last path in the file
    if (m_file->isLastPath(m_path)) {
        m_withNav = true;
        m_navTree = m_file->getNavTree();
    }
}

void RootFileWriter::resetAddress()
{
    m_headerAddr = 0;
    m_eventAddr = 0;
    m_navAddr = 0; 
}
