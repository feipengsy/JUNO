#include "RootIOSvc/RootFileWriter.h"
#include "SniperKernel/SniperLog.h"
#include "EvtNavigator/EvtNavigator.h"
#include "RootIOUtil/TreeMetaData.h"
#include "RootIOUtil/RootOutputFileManager.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"

#include "TObject.h"
#include "TTree.h"
#include "TFile.h"
#include "TProcessID.h"
#include "TDirectory.h"

RootFileWriter::RootFileWriter(const std::string& treepath, const std::string& headername, const std::string& eventname)
    : m_file(0)
    , m_tree(0)
    , m_navTree(0)
    , m_treeMetaData(0)
    , m_dir(0)
    , m_headerName(headername)
    , m_eventName(eventname)
    , m_path(treepath)
    , m_withNav(false)
    , m_entries(0)
    , m_fileEntries(0)
    , m_headerAddr(0)
    , m_eventAddr(0)
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
    if (!m_headerAddr || !m_eventAddr || !m_navAddr) {
        LogError << "Address not set, can not write"
                 << std::endl;
        return false;
    }

    bool write = static_cast<JM::EvtNavigator*>(m_navAddr)->writePath(m_path);
    if ("unknown" == m_headerName || !write) {
        // Currently unknown stream or skipped entry, just idling
        ++m_fileEntries;
        if (m_withNav) {
            this->writeNav();
        }
        return true;  
    }

    if (!m_tree) {
        bool ok = this->initialize();
        if (!ok) {
            LogError << "Fail to initialize RootFileWriter"
                     << std::endl;
            return false;
        }
    }


    // Build auto-loading data for TreeMetaData
    TObject* header = static_cast<TObject*>(m_headerAddr);
    // Header in the 1st branch, event in the 2nd branch
    // Now, branch ID will NOT be saved
    fillBID(header, -1);

    TObject* event = static_cast<TObject*>(m_eventAddr);
    fillBID(event, -1);

    // Set entry for SmartRefs
    JM::EvtNavigator* nav = static_cast<JM::EvtNavigator*>(m_navAddr);
    nav->setHeaderEntry(m_path, m_entries);

    JM::HeaderObject* rheader = static_cast<JM::HeaderObject*>(m_headerAddr);
    rheader->setEventEntry(m_entries);

    bool ok = this->writeData();
    if (!ok) return false;
    if (m_withNav) ok = this->writeNav();
    if (!ok) return false;

    ++m_entries;
    ++m_fileEntries;
    this->resetAddress();    

    return true;
}

bool RootFileWriter::writeData()
{
    int nbytes = m_tree->Fill();
    LogDebug <<  "Wrote " << nbytes
             << " bytes to entry " << m_entries
             << " of tree " << m_path
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

bool RootFileWriter::initialize()
{
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
}

void RootFileWriter::setAddress(void* nav, void* header, void* event)
{
    m_navAddr = nav;
    m_headerAddr = header;
    m_eventAddr = event;
}

void RootFileWriter::setHeaderName(const std::string& name)
{
    m_headerName = name;
}

void RootFileWriter::setEventName(const std::string& name)
{
    m_eventName = name;
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
