#include "RootIOSvc/RootFileWriter.h"
#include "SniperKernel/SniperLog.h"
#include "EvtNavigator/EvtNavigator.h"
#include "Event/HeaderObject.h"
#include "RootIOUtil/TreeMetaData.h"
#include "RootIOUtil/RootOutputFileManager.h"

#include "TTree.h"
#include "TFile.h"
#include "TProcessID.h"
#include "TDirectory.h"

OutputTreeHandle::OutputTreeHandle(const std::string& path, const std::string& objName)
    : m_path(path)
    , m_objName(objName)
    , m_fullTreeName(path + objName.substr(objName.rfind("::")+1))
    , m_tree(0)
    , m_addr(0)
    , m_entries(0)
{
}

OutputTreeHandle::~OutputTreeHandle()
{
    // Yes, we don't delete TTree*
}

bool OutputTreeHandle::fill(int& nbytes)
{
    if (!m_addr) {
        return false;
    }
    if (!m_tree) {
        // Create tree first
        std::string title = "Tree at " + m_path + " holding " + m_objName;
        std::string treeName = m_objName.substr(m_objName.rfind("::")+1);
        std::string branchName = treeName;
        m_tree = new TTree(treeName.c_str(), title.c_str());
        m_tree->Branch(branchName.c_str(), m_objName.c_str(), &m_addr,16000,1);
    }
    nbytes = m_tree->Fill();
    ++m_entries;
    this->fillUID();
    return true;
}

void OuputTreeHandle::write()
{
    if (m_tree) {
        m_tree->Write(NULL,TObject::kOverwrite);
    }
}

void OutputTreeHandle::fillUID(int bid = -1)
{
    TObject* obj = static_cast<TObject*>(m_addr);
    UInt_t uid = obj->GetUniqueID();
    TProcessID* pid = TProcessID::GetProcessWithUID(uid,obj);
    const char* guid = pid->GetTitle();
    int iid;
    StringVector::const_iterator posPID = find( m_guid.begin(), m_guid.end(), guid);
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

//---------------------- RootFileWriter ------------------------

RootFileWriter::RootFileWriter(const std::string& path, const std::string& headerName)
    : m_file(0)
    , m_headerTree(0)
    , m_navTree(0)
    , m_dir(0)
    , m_path(path[path.length()+1] == '/' ? path : path + '/' )
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

    // Try to initialize if nessassry
    if (!m_initialized) {
        this->initialize();
    }

    bool write = m_navAddr->writePath(m_path);
    if (!m_initialized || !write) {
        // Currently unknown stream or skipped entry, just idling
        ++m_fileEntries;
        if (m_withNav) {
            this->writeNav();
        }
        return true;  
    }

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
    // Set entry number
    m_navAddr->setHeaderEntry(m_path, m_headerTree->entries());

    int nbytes = 0;
    // Header will always be written
    m_headerTree->fill(nbytes);
    LogDebug <<  "Wrote " << nbytes
             << " byte(s) to entry " << m_entries
             << " of header of " << m_path
             << std::endl;
    return nbytes > 0;
}

bool RootFileWriter::writeEvent()
{
    String2TreeHandle::iterator it, end = m_eventTrees.end();
    for (it = m_eventTrees.begin(); it != end; ++it) {
        int nbytes = 0;
        bool write = it->second->fill(nbytes);
        if (!write) continue;

        // Set entry number of this event
        m_navAddr->getHeader(m_path)->setEventEntry(it->first, it->second->entries() - 1);

        LogDebug << "Wrote " << nbytes
                 << " byte(s) to entry " << it->entries()
                 << " of " << it->objectName()
                 << " of path " << m_path
                 << std::endl;
        if (!nbytes) {
            return false;
        }
    }
    return true;
}

bool RootFileWriter::writeNav()
{
    if (0 == m_fileEntries) {
        // TODO do we need to reset this?
        // Tell FileMetaData what paths EvtNavigator privides.
        std::vector<std::string> eventNames, paths = m_navAddr->getPath();
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
    m_headerTree->write();
    String2TreeHandle::iterator it, end = m_eventTrees.end();
    for (it = m_eventTrees.begin(); it != end; ++it) {
        it->second->write();
    }

    // Tree for EvtNavigator will be written by file handle
    //if (m_withNav) m_navTree->Write(NULL,TObject::kOverwrite);

    // Set TreeMetaData and lazy-loading data
    TMDVector::iterator tit, tend = m_treeMetaDatas.end();
    for (tit = m_treeMetaDatas.begin(); tit != tend; ++tit) {
        m_file->addTreeMetaData(*tit);
    }
    //m_file->addUniqueIDTable(m_path, m_guid, m_uid, m_bid);
    //m_file->addUUID(m_guid);

    // Dec file reference, close when it hits 0
    RootOutputFileManager::get()->close_file(m_file->getName());

    // Reset pointers and clean gabage
    m_file = 0;
    delete m_headerTree;
    m_headerTree = 0;
    for (it = m_eventTrees.begin(); it != end; ++it) {
        delete it->second;
    }
    m_eventTrees.clear();
    m_navTree = 0;
    m_dir = 0;
    m_withNav = false;
    // Reset file entry
    m_fileEntries = 0;
    m_initialized = false;
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
    // Make the directories up to the path
    TFile* rootFile =  m_file->getFile();
    m_dir = rootFile;
    std::string::size_type last = 0, slash = m_path.find('/');
    for (; slash != std::string::npos; slash = tempPpath.find('/',last)) {
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
    rootFile->cd(m_path.c_str());

    // Create the OutputTreeHandle for header
    m_headerTree = new OutputTreeHandle(m_path, m_headerName);
    // Create the OutputTreeHandle for event
    const std::vector<std::string>& eventNames = EDMManager::get()->getEventNameWithHeader(m_headerName);
    for (std::vector<std::string>::const_iterator it = eventNames.begin(); it != eventNames.end(); ++it) {
        m_eventTrees.insert(std::make_pair(*it, new OutputTreeHandle(m_path, *it)));
        JM::TreeMetaData* etmd = new JM::TreeMetaData();
        etmd->SetTreeName(m_path + it->substr(it->rfind("::") + 1))
    }

    JM::TreeMetaData* htmd = new JM::TreeMetaData();
    htmd->SetTreeName(m_path + m_headerName.substr(m_headerName.rfind("::") + 1));
    m_treeMetaDatas.push_back(htmd);

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
    m_navAddr = 0; 
    m_headerTree->resetAddress();
    String2TreeHandle::iterator it, end = m_eventTrees.end();
    for (it = m_eventTrees.begin(); it != end; ++it) {
        it->second->resetAddress();
    }
}
