#include "SmartRefTableImpl.h"
#include "TProcessID.h"

#include "UniqueIDTable"
#include <algorithm>

SmartRefTableImpl::SmartRefTableImpl(Int_t fileid) 
    : m_TreeIDs(0)
    , m_PreIid(-1)
    , m_N(0)
    , m_NumPIDs(0)
    , m_AllocSize(0)
    , m_MinSize(1000)
    , m_MaxSize(100000)
    , m_FileID(fileid)
{
  // Default constructor of SmartRefTableImpl
}

SmartRefTableImpl::~SmartRefTableImpl()
{
  // Destructor
  delete [] m_AllocSize;
  delete [] m_N;
  for (Int_t pid = 0; pid < m_NumPIDs; ++pid) {
    delete [] m_TreeIDs[pid];
  }
  delete [] m_TreeIDs;
}

void SmartRefTableImpl::Add(const std::string& guid, Int_t uid, Int_t bid, Int_t tid)
{
  // Add a referenced object to SmartRefTableImpl
  // Called by SmartRefTable::ReadMetaData()

  // get or generate a iid for a TProcessID title 
  Int_t iid = GetInternalIdxForPID( guid );

  Int_t newsize = 0;
  uid = uid & 0xffffff;
  // expand fTreeIDs if necessary
  if (uid >= m_AllocSize[iid]) {
    newsize = uid + std::min(uid / 2, m_MaxSize);
    if (newsize < m_MinSize)
      newsize = m_MinSize;
    newsize = ExpandForIID(iid, newsize);
  }
  if (newsize < 0) {
    // fail to expand or fTreeIDs
    return;
  }
  m_TreeIDs[iid][uid] = tid + 1 + ( bid + 1  << 24 );
  if (uid >= m_N[iid]) m_N[iid] = uid + 1;
}

Int_t SmartRefTableImpl::AddInternalIdxForPID(const std::string& guid)
{
  // Add the internal index for fProcessIDs, fAllocSize, etc given a guid.

  if (guid == m_PrePID) return m_PreIid; // use cached iid
  Int_t iid = FindPIDGUID(guid);
  if (iid == -1) {
    // new guid
    m_ProcessGUIDs.push_back(guid);
    iid = m_ProcessGUIDs.size() - 1;
  }
  ExpandPIDs(iid + 1);

  // cache the guid and iid
  m_PrePID = guid;
  m_PreIid = iid;
  return iid;
}

void SmartRefTableImpl::Clear()
{
  // Clear the table and reclaim memory

  delete [] m_AllocSize;
  m_AllocSize = 0;
  delete [] m_N;
  m_N = 0;
  for (Int_t pid = 0; pid < m_NumPIDs; ++pid) {
    delete [] m_TreeIDs[pid];
  }
  delete [] m_TreeIDs;
  m_TreeIDs = 0;

  m_NumPIDs = 0;
  m_PreIid = -1;
  m_PrePID.clear();
  m_ProcessGUIDs.clear();
}

void SmartRefTableImpl::Reset()
{
  // Clear the table, but won't reclaim memory

  for (Int_t iid = 0; iid < m_NumPIDs; ++iid) {
    memset(m_TreeIDs[iid], 0, sizeof(Int_t) * m_N[iid]);
  }
  memset(m_N, 0, sizeof(Int_t) * m_NumPIDs);
  m_PreIid = -1;
  m_PrePID.clear();
}

void SmartRefTableImpl::ExpandPIDs(Int_t numpids)
{
  //Expand the arrays of managed PID

  if (numpids <= m_NumPIDs) return;

  // else add to internal tables
  Int_t oldNumPIDs = m_NumPIDs;
  m_NumPIDs  = numpids;

  // update m_AllocSize
  Int_t *allocTemp = m_AllocSize;
  m_AllocSize = new Int_t[m_NumPIDs];
  if (allocTemp)
    memcpy(m_AllocSize, allocTemp, oldNumPIDs * sizeof(Int_t));
  memset(&m_AllocSize[oldNumPIDs], 0, (m_NumPIDs - oldNumPIDs) * sizeof(Int_t));
  delete [] allocTemp;

  // update m_N 
  Int_t* nTemp = m_N;
  m_N = new Int_t[m_NumPIDs];
  if (nTemp)
    memcpy(m_N, nTemp, oldNumPIDs * sizeof(Int_t));
  memset(&m_N[oldNumPIDs], 0, (m_NumPIDs - oldNumPIDs) * sizeof(Int_t));
  delete nTemp;

  // update m_TreeIDs
  Int_t** treeIDsTemp = m_TreeIDs;
  m_TreeIDs = new Int_t *[m_NumPIDs];
  if (treeIDsTemp)
    memcpy(m_TreeIDs, treeIDsTemp, oldNumPIDs * sizeof(Int_t *));
  memset(&m_TreeIDs[oldNumPIDs], 0, (m_NumPIDs - oldNumPIDs) * sizeof(Int_t*));
}


Int_t SmartRefTableImpl::ExpandForIID(Int_t iid, Int_t newsize)
{
  // Expand m_TreeIDs to newsize for internel ProcessID index iid

  if (newsize < 0)
    return newsize;
  if (newsize != m_AllocSize[iid]) {
    Int_t *temp = m_TreeIDs[iid];
    if (newsize != 0) {
      m_TreeIDs[iid] = new Int_t[newsize];
      if (newsize < m_AllocSize[iid]) {
        // contract, just copy old values
        memcpy(m_TreeIDs[iid], temp, newsize * sizeof(Int_t));
      }
      else {
        // expand, copy old values and set new values to 0
        memcpy(m_TreeIDs[iid], temp, m_AllocSize[iid] * sizeof(Int_t));
        memset(&m_TreeIDs[iid][m_AllocSize[iid]], 0, (newsize - m_AllocSize[iid]) * sizeof(Int_t));
      }
    }
    else {
      m_TreeIDs[iid] = 0;
    }
    if (m_AllocSize[iid])
      delete [] temp;
    m_AllocSize[iid] = newsize;
  }
  return newsize;
}

Int_t SmartRefTableImpl::FindPIDGUID(const std::string& guid) const
{
  // Get fProcessGUIDs' index of the TProcessID with GUID guid
  std::vector<std::string>::const_iterator posPID = std::find(m_ProcessGUIDs.begin(), m_ProcessGUIDs.end(), guid);
  if (posPID == m_ProcessGUIDs.end()) return -1;
  return posPID - m_ProcessGUIDs.begin();
}

Int_t SmartRefTableImpl::GetBranchID(Int_t uid, const TProcessID* pid)
{
  Int_t iid = GetInternalIdxForPID(pid->GetTitle());

  uid = uid & 0xFFFFFF;
  if (uid < 0 || uid >= m_N[iid]) return -1;
  return ( m_TreeIDs[iid][uid] >> 24 ) - 1;
}

Int_t SmartRefTableImpl::GetTreeID(Int_t uid, const TProcessID* pid)
{
  Int_t iid = GetInternalIdxForPID(pid->GetTitle());

  uid = uid & 0xFFFFFF;
  if (uid < 0 || uid >= m_N[iid]) return -1;
  return ( m_TreeIDs[iid][uid] & 0xFFFFFF ) - 1;
}

Int_t SmartRefTableImpl::GetFileID()
{
  return m_FileID;
}

Int_t SmartRefTableImpl::GetInternalIdxForPID(const std::string& guid) const
{
  // Get the index for m_ProcessIDs

  return const_cast <SmartRefTableImpl*>(this)->AddInternalIdxForPID(guid);
}

void SmartRefTableImpl::ReadMetaData(JM::TablePerTree* table, Int_t treeid)
{
  // Read UniqueIDTable

  //BIDVector: std::vector<std::vector<Short_t> >
  TablePerTree::BIDVector bids = table->GetBranchIDs();
  //UIDVector: std::vector<std::vector<Int_t> > 
  TablePerTree::UIDVector uids = table->GetUniqueIDs();
  //GUIDVector: std::vector<std::string> 
  TablePerTree::GUIDVector guids = table->GetGUIDs();

  TablePerTree::UIDVector::const_iterator it_uids;
  std::vector<Int_t>::const_iterator it_uid;
  TablePerTree::GUIDVector::const_iterator it_guids;
  if (0 != bids.size()) {
    TablePerTree::BIDVector::const_iterator it_bids;
    std::vector<Short_t>::const_iterator it_bid;
    for (it_uids = uids.begin(), it_bids = bids.begin(), it_guids = guids.begin(); it_guids != guids.end(); ++it_uids, ++it_bids, ++it_guids) {
      for (it_uid = it_uids->begin(), it_bid = it_bids->begin(); it_uid != it_uids->end(); ++it_uid, ++it_bid) {
        Add(*it_guids, *it_uid, *it_bid, treeid);
      }
    }
  }
  else {
    for (it_uids = uids.begin(), it_guids = guids.begin(); it_guids != guids.end(); ++it_uids, ++it_guids) {
      for (it_uid = it_uids->begin(); it_uid != it_uids->end(); ++it_uid) {
        Add(*it_guids, *it_uid, 0, treeid);
      }
    }
  }
}
