#include "SmartRefTable.h"
#include "SmartRefTableImpl.h"

SmartRefTable* SmartRefTable::fgSmartRefTable = 0;

SmartRefTable::SmartRefTable()
{
    // Default constructor of SmartRefTable

    fgSmartRefTable = this;
}

SmartRefTable::~SmartRefTable()
{
    // Destructor
    for (SRTIterator it = m_tableList.begin();it != m_tableList.end(); ++it) {
        delete *it;
    }
    if (fgSmartRefTable == this) fgSmartRefTable = 0;
}

Int_t SmartRefTable::GetBranchID(Int_t uid, const TProcessID* pid)
{
    int branchID = -1;
    for (SRTIterator it = m_tableList.begin();it != m_tableList.end(); ++it) {
        branchID = (*it)->GetBranchID(uid, pid);
        if (-1 != branchID) return branchID;
    }
    return branchID;
}

Int_t SmartRefTable::GetTreeID(Int_t uid, const TProcessID* pid)
{
    int treeID = -1;
    for (SRTIterator it = m_tableList.begin();it != m_tableList.end(); ++it) {
        treeID = (*it)->GetTreeID(uid, pid);
        if (-1 != treeID) return treeID;
    }
    return treeID;
}

Int_t SmartRefTable::GetOffset(Int_t uid, const TProcessID* pid)
{
    Int_t offset = -1;
    for (SRTIterator it = m_tableList.begin();it != m_tableList.end(); ++it) {
        offset = (*it)->GetOffset(uid, pid);
        if (-1 != offset) return offset;
    }
    return offset;
}

SmartRefTable* SmartRefTable::GetRefTable()
{
    //Static function returning the current SmartRefTable.
  
    return fgSmartRefTable;
}

void SmartRefTable::StartNewTable(Int_t fileid)
{
    m_tableList.push_back(new SmartRefTableImpl(fileid));
}

void SmartRefTable::ReadMetaData(JM::TablePerTree* table, Int_t treeid, const std::vector<Long64_t>& breakPoints)
{
    m_tableList.back()->ReadMetaData(table, treeid, breakPoints);
}

void SmartRefTable::DeleteTable(Int_t fileid)
{
    SRTIterator it, end = m_tableList.end();
    for (it = m_tableList.begin();it != end; ++it) {
        int fileID = (*it)->GetFileID();
        if (fileid == fileID) {
           delete *it;
           m_tableList.erase(it);
           break;
        }
    }
}

bool SmartRefTable::InspectFileID(Int_t fileid)
{
    SRTIterator it, end = m_tableList.end();
    for (it = m_tableList.begin();it != end; ++it) {
        if (fileid == (*it)->GetFileID()) {
            return true;
        }
    }
    return false;
}
