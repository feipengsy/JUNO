#include "RootIOUtil/InputElementKeeper.h"
#include "RootIOUtil/InputTreeManager.h"
#include "RootIOUtil/InputFileManager.h"
#include "RootIOUtil/SmartRefTable.h"
#include "RootIOUtil/RootFileReader.h"
#include "RootIOUtil/FileMetaData.h"
#include "RootIOUtil/UniqueIDTable.h"

#include "TTree.h"
#include "TFile.h"
#include "TProcessID.h"

InputElementKeeper* InputElementKeeper::m_keeper = 0;

InputElementKeeper::InputElementKeeper()
            : m_table(new SmartRefTable)
            , m_fileMgr(new InputFileManager)
            , m_refCount(0)
{
    m_keeper = this;
}

InputElementKeeper::~InputElementKeeper()
{
    delete m_table;
    delete m_fileMgr;
    if ( this == m_keeper ) {
        m_keeper = 0;
    }
}

void InputElementKeeper::AddRef()
{
    ++m_refCount;
}

void InputElementKeeper::ClearTable()
{
    String2FileIDs::iterator list = m_uuid2FileList.find(m_tempUUID);
    if (list != m_uuid2FileList.end()) {
        std::vector<int>::iterator it, end = list->second.end();
        for (it = list->second.begin(); it != end; ++it) {
            m_table->DeleteTable(*it);
        }
    }
}

void InputElementKeeper::DecRef()
{
    if (0 == --m_refCount) {
        delete this;
    }
}

int InputElementKeeper::RegisterFile(const std::string& filename, const JM::FileMetaData* metadata)
{

    int fileid = m_fileMgr->FindFile(filename);
    if (fileid != -1) {
        // File already registered, do nothing.
        return fileid;
    }
    // Register file
    fileid = m_fileMgr->AddFile(filename);

    // Register trees
    const std::vector<JM::TreeMetaData*>& trees = metadata->GetTreeMetaData();
    BOOST_FOREACH(const JM::TreeMetaData* tmd, trees) {
        // Check if need to create a passive stream
        const std::string& treeName = tmd->GetTreeName();
        PassiveStream* ps = 0;
        PSMap::iterator pos = m_psMap.find(treeName);
        if (pos == m_psMap.end()) {
            ps = new PassiveStream;
            m_psMap.insert(std::make_pair(treeName, ps));
        }
        else { 
            ps = pos->second;
        }
        // Add this tree to the certain passive stream
        int treeid = TreeInfoList.size();
        int treeIndex = ps->AddTree(fileid, fmd->GetBreakPoints()[treeName]);
        // Create a treeInfo
        TreeInfoList.push_back(new TreeInfo(ps, treeIndex, fileid));
        // Tell fileMgr about the info of this tree.
        // So that when a file need to be re-opened, we can update passive streams with new tree pointer
        m_fileMgr->AddTreeInfo(treeid, treeName);
        // Add this file to the path2fileid map
        // Only need dir path, burn tree name
        std::string path = treeName.subdir(0, treeName.rfind('/'));
        if (m_path2FileList.find(path) == m_path2FileList.end()) {
            m_path2FileList.insert(std::make_pair(path, std::vector<int>()));
        }
        // TODO redupiclated fileid?
        m_path2FileList(path).push_back(fileid);
    }
    // Add this file to the uuid2fileid map
    const JM::FileMetaData::StringVector& uuidList = fmd->GetUUIDList();
    BOOST_FOREACH(const std::string& uuid, uuidList) {
        if (m_uuid2FileList.find(uuid) == m_uuid2FileList.end()) {
            m_uuid2FileList.insert(std::make_pair(uuid, std::vector<int>()));
        }
        m_uuid2FileList(uuid).push_back(fileid);
    }
    return fileid;
}

void InputElementKeeper::AddObjRef(Int_t uid, const TProcessID* pid)
{
    int treeid = m_table->GetTreeID(uid, pid);
    // Unregistered object
    if (-1 == treeid) {
        return;
    }
    int fileid = m_treeInfoList[treeid]->fileID;
    m_fileMgr->AddObjRef(fileid);
}

void InputElementKeeper::DecObjRef(Int_t uid, const TProcessID* pid)
{
    int treeid = m_table->GetTreeID(uid, pid);
    // Unregistered object
    if (-1 == treeid) {
        return;
    }
    int fileid = m_treeInfoList[treeid]->fileID;
    m_fileMgr->DecObjRef(fileid);
}


void InputElementKeeper::AddTreeRef(int fileid)
{
    m_fileMgr->AddTreeRef(fileid);
}

void InputElementKeeper::DecTreeRef(int fileid)
{
    m_fileMgr->DecTreeRef(fileid);
}

InputElementKeeper* InputElementKeeper::GetInputElementKeeper(bool create)
{
    if (!m_keeper && create) {
        m_keeper = new InputElementKeeper;
    }
    return m_keeper;
}

bool InputElementKeeper::CheckFileStatus(int fileid) const
{
    return m_fileMgr->CheckFileStatus(fileid);
}

bool InputElementKeeper::GetNavTree(int fileid, TTree*& tree)
{
    if (!CheckFileStatus(fileid)) {
        OpenFile(fileid); // TODO rubust check: fail to open file
    }
    TFile* file = m_fileMgr->GetFile(fileid);
    tree = RootFileReader::GetNavTree(file);
    return true;
}

std::string& InputElementKeeper::GetFileName(int fileid)
{
    return m_fileMgr->GetFileName(fileid);
}

TFile* InputElementKeeper::GetFile(int fileid)
{
    return m_fileMgr->GetFile(fileid);
}

void InputElementKeeper::LoadUniqueID(int fileid)
{
    if (m_table->InspectFileID(fileid)) {
        // Meta data already in-momory, do nothing
        return;
    }

    m_table->StartNewTable(fileid);
    JM::UniqueIDTable* uidTable = 0;
    bool preStatus = CheckFileStatus(fileid);
    // TODO robust check: fail to get uidTable
    if (preStatus) {
        uidTable = RootFileInterface::GetUniqueIDTable(GetFile(fileid));
    }
    else {
        TFile* file = RootFileReader::OpenFile(GetFileName(fileid));
        uidTable = RootFileReader::GetUniqueIDTable(file);
        // Only open file when start to read object
        file->Close();
    }
    // Map of tree id to tree name.
    std::map<int,std::string> treeInfo = m_fileMgr->GetTreeInfo(fileid);
    // TableMap : std::map<std::string, TablePerTree*>
    JM::UniqueIDTable::TableMap tables = uidTable->GetTable();
    std::map<int,std::string>::iterator it, end = treeInfo.end();
    for (it = treeInfo.begin(); it != end; ++it) {
        JM::UniqueIDTable::TableMap::iterator tpos = tables.find(it->second);
        if (tpos != tables.end()) {
            const std::vector<Long64_t>& breakPoints = m_treeInfoList[it->first]->breakPoints;
            m_table->ReadMetaData(/*TablePerTree*/tpos->second, /*treeID*/it->first, breakPoints);
        }
    }
    delete uidTable;
}

void InputElementKeeper::OpenFile(int fileid)
{
    const std::string& filename = m_fileMgr->GetFileName(fileid);
    const std::map<int,std::string>& treeInfo = m_fileMgr->GetTreeInfo(fileid);

    TFile* file = RootFileInterface::OpenFile(filename);
    std::map<int,std::string>::const_iterator treeInfoIt;
    for (treeInfoIt = treeInfo.first(); treeInfoIt != treeInfo.end(); ++treeInfoIt) {
        // Update PassiveStream
        TTree* tree = RootFileInterface::GetTree(treeInfo->second);
        int index =  m_treeInfoList[treeInfoIt->first]->streamIndex;
        PassiveStream* stream = m_treeInfoList[treeInfoIt->first]->stream;
        stream->updateTree(index, tree);
    }
    // Update InputFileHandle
    m_fileMgr->UpdateFile(fileid, file);
    // Load unique id data if nesessary
    this->LoadUniqueID(fileid);
}

void InputElementKeeper::ReadObject(Int_t uid, const TProcessID* pid, Long64_t entry)
{
    int treeid = m_table->GetTreeID(uid, pid);
    if (-1 == treeid) {
        // Try to load meta data from other files and search again
        String2FileIDs::iterator pos = m_uuid2FileList.find(pid->GetTitle());
        if (pos != m_uuid2FileList.end()) {
            std::vector<int>::iterator it, end = pos->second.end();
            // Open rest file that holds same TProcessID uuid
            for (it = pos->second.begin(); it != end; ++it) {
                // If meta data of this file is not in memory, load it
                this->LoadUniqueID(*it);
            }
            // Search tree id again
            treeid = m_table->GetTreeID(uid, pid);
        }
        // Sorry, the object is not in the input file list
        if (-1 == treeid) {
            return 0;
        }
    }
    // Tree id is found, now try to load object
    // Check status of the file holding the tree
    int fileID = TreeInfoList[treeID]->fileID;
    bool status = this->CheckFileStatus(fileID);
    if (!status) {
        // File is closed, need to open
        this->OpenFile(fileID);
    }
    Long64_t offset = 0;
    int offsetIndex = m_table->GetOffset(uid, pid);
    if (-1 != offsetIndex) {
        offset = m_treeInfoList[treeid]->GetTreeOffset(offsetIndex);
    }
    TreeInfoList[treeID]->stream->ReadObject(entry + offset);
}

void InputElementKeeper::Notify(int option, Int_t uid, const TProcessID* pid, Long64_t entry)
{
    switch (option) {
        case : Read
            ReadObject(uid, pid, entry);
            break;
        case : New
            AddObjRef(uid, pid);
            break;
        case : Delete
            DecObjRef(uid, pid);
            break;
    }
}

std::vector<int> InputElementKeeper::GetFileList(const std::string& path)
{
    if (path == "none") {
        // Return all the files
        std::vector<int> allList;
        String2FileIDs::iterator it, end = m_path2FileList.end();
        for (it = m_path2FileList.begin(); it != end; ++it) {
            allList.insert(allList.begin(), it->second.begin(), it->second.end());
        }
        return allList;
    }
    String2FileIDs::iterator pos = m_path2FileList.find(path);
    if (pos == m_path2FileList.end()) {
        return std::vector<int>();
    }
    return pos->second;
}
