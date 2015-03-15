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
            , m_treeMgr(new InputTreeManager)
            , m_fileMgr(new InputFileManager)
            , m_refCount(0)
{
    m_keeper = this;
}

InputElementKeeper::~InputElementKeeper()
{
    delete m_table;
    delete m_treeMgr;
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

int InputElementKeeper::RegisterFile(const std::string& filename, const std::vector<JM::TreeMetaData*>& trees)
{
    int fileid = m_fileMgr->FindFile(filename);
    // File already registered.
    if (fileid != -1) {
        return fileid;
    }
    fileid = m_fileMgr->AddFile(filename);
    std::map<int,std::string> treeinfo;
    std::vector<JM::TreeMetaData*>::const_iterator it;
    for (it = trees.begin(); it != trees.end(); ++it) {
        int treeid = m_treeMgr->AddTree(fileid);
        // Register meta data later when start to read data
        //m_table->ReadMetaData(*it, treeid);
        treeinfo.insert(std::make_pair(treeid, (*it)->GetTreeName()));
    }
    m_fileMgr->SetTreeInfo(fileid, treeinfo);
    return fileid;
}

void InputElementKeeper::RegisterPathMap(const String2FileIDs& pathmap)
{
    RegisterFileMap(pathmap, "path");
}

void InputElementKeeper::RegisterUUIDMap(const String2FileIDs& uuidmap)
{
    RegisterFileMap(uuidmap, "uuid");
}

void InputElementKeeper::RegisterFileMap(const String2FileIDs& value, const std::string& type)
{
    String2FileIDs* tar = 0;
    if ("uuid" == type) {
        tar = &m_uuid2FileList;
    }
    else if ("path" == type) {
        tar = &m_path2FileList;
    }
    else {
        return;
    }

    String2FileIDs::iterator pos;
    String2FileIDs::const_iterator it, end = value.end();
    for (it = value.begin(); it != end; ++it) {
      pos = tar->find(it->first);
      if (pos != tar->end()) {
          // Hit, connect the 2 file id vector
          pos->second.insert(pos->second.end(), it->second.begin(), it->second.end());
      }
    else {
      // Miss, insert one new element
      tar->insert(std::make_pair(it->first, it->second));
    }
  }
}

void InputElementKeeper::AddObjRef(Int_t uid, const TProcessID* pid)
{
    int treeid = m_table->GetTreeID(uid, pid);
    // Unregistered object
    if (-1 == treeid) {
        return;
    }
    m_treeMgr->AddRef(treeid);
}

void InputElementKeeper::AddTreeRef(int fileid)
{
    m_fileMgr->AddTreeRef(fileid);
}

void InputElementKeeper::DecTreeRef(int fileid)
{
    m_fileMgr->DecTreeRef(fileid);
}

void InputElementKeeper::SetNavTreeRef(int fileid)
{
    m_fileMgr->SetNavTreeRef(fileid);
}

void InputElementKeeper::ResetNavTreeRef(int fileid)
{
    m_fileMgr->ResetNavTreeRef(fileid);
}

void InputElementKeeper::DelObj(Int_t uid, TProcessID* pid, Long64_t entry)
{
    int treeid = m_table->GetTreeID(uid, pid);
    if (-1 == treeid) {
        return;
    }
    m_tempUUID = pid->GetTitle();
    m_treeMgr->DelObj(treeid, entry);
    m_tempUUID.clear();
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
    std::map<int,std::string> treeInfo = m_fileMgr->GetTreeInfo(fileid);

    m_table->StartNewTable(fileid);
    JM::UniqueIDTable* uidTable = 0;
    bool preStatus = CheckFileStatus(fileid);
    // TODO robust check: fail to get uidTable
    if (preStatus) {
        uidTable = RootFileReader::GetUniqueIDTable(GetFile(fileid));
    }
    else {
        TFile* file = RootFileReader::OpenFile(GetFileName(fileid));
        uidTable = RootFileReader::GetUniqueIDTable(GetFile(fileid));
        // Do not leave the file opened
        file->Close();
    }
    // TableMap : std::map<std::string, TablePerTree*>
    JM::UniqueIDTable::TableMap tables = uidTable->GetTable();
    std::map<int,std::string>::iterator it, end = treeInfo.end();
    for (it = treeInfo.begin(); it != end; ++it) {
        JM::UniqueIDTable::TableMap::iterator tpos = tables.find(it->second);
        if (tpos != tables.end()) {
            m_table->ReadMetaData(tpos->second, it->first);
        }
    }
    delete uidTable;
}

void InputElementKeeper::OpenFile(int fileid)
{
    std::string filename = m_fileMgr->GetFileName(fileid);
    TFile* file = 0;
    std::map<int,std::string> treeInfo = m_fileMgr->GetTreeInfo(fileid);
    std::vector<TTree*> trees;
    if (!RootFileReader::ReOpen(filename, file, treeInfo, trees)) {
        //TODO robust check: failed to open file and get trees
        return;
    }
    // Update InputFileHandle and InputTreeHandles
    m_fileMgr->UpdateFile(fileid, file);
    std::map<int,std::string>::iterator it1 = treeInfo.begin();
    std::vector<TTree*>::iterator it2 = trees.begin();
    // The vector of poniters to TTree is of the same order with the treeInfo map
    for (; it1 != treeInfo.end(); ++it1, ++it2) {
        // Reset the pointer to TTree
        m_treeMgr->ResetTree(it1->first, *it2);
    }
    // Load meta data if nesessary
    this->LoadUniqueID(fileid);
}

TBranch* InputElementKeeper::GetBranch(Int_t uid, const TProcessID* pid, Long64_t& offset, Int_t branchID)
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
    TTree* tree = m_treeMgr->GetTree(treeid);
    offset = m_table->GetOffset(uid, pid);
    if (!tree) {
        return 0;
    }
    if (-1 == branchID) {
        branchID = m_table->GetBranchID(uid, pid);
    }
    if (-1 == branchID) {
        return 0;
    }
    return (TBranch*)tree->GetListOfBranches()->At(branchID);
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
