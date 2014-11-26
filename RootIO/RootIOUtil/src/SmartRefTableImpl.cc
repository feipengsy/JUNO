#include "SmartRefTableImpl.h"
#include "TProcessID.h"

#include "RootIOUtil/TreeMetaData.h"
#include <algorithm>

SmartRefTableImpl::SmartRefTableImpl() : fTreeIDs(0), fPreIid(-1), fN(0), fNumPIDs(0), fAllocSize(0), fMinSize(1000), fMaxSize(100000)
{
  // Default constructor of SmartRefTableImpl
}

SmartRefTableImpl::~SmartRefTableImpl()
{
  // Destructor
  delete [] fAllocSize;
  delete [] fN;
  for (Int_t pid = 0; pid < fNumPIDs; ++pid) {
    delete [] fTreeIDs[pid];
  }
  delete [] fTreeIDs;
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
  if (uid >= fAllocSize[iid]) {
    newsize = uid + std::min(uid / 2, fMaxSize);
    if (newsize < fMinSize)
      newsize = fMinSize;
    newsize = ExpandForIID(iid, newsize);
  }
  if (newsize < 0) {
    // fail to expand or fTreeIDs
    return;
  }
  fTreeIDs[iid][uid] = tid + 1 + ( bid + 1  << 24 );
  if (uid >= fN[iid]) fN[iid] = uid + 1;
}

Int_t SmartRefTableImpl::AddInternalIdxForPID(const std::string& guid)
{
  // Add the internal index for fProcessIDs, fAllocSize, etc given a guid.

  if (guid == fPrePID) return fPreIid; // use cached iid
  Int_t iid = FindPIDGUID(guid);
  if (iid == -1) {
    // new guid
    fProcessGUIDs.push_back(guid);
    iid = fProcessGUIDs.size() - 1;
  }
  ExpandPIDs(iid + 1);

  // cache the guid and iid
  fPrePID = guid;
  fPreIid = iid;
  return iid;
}

void SmartRefTableImpl::Clear()
{
  // Clear the table and reclaim memory

  delete [] fAllocSize;
  fAllocSize = 0;
  delete [] fN;
  fN = 0;
  for (Int_t pid = 0; pid < fNumPIDs; ++pid) {
    delete [] fTreeIDs[pid];
  }
  delete [] fTreeIDs;
  fTreeIDs = 0;

  fNumPIDs = 0;
  fPreIid = -1;
  fPrePID.clear();
  fProcessGUIDs.clear();
}

void SmartRefTableImpl::Reset()
{
  // Clear the table, but won't reclaim memory

  for (Int_t iid = 0; iid < fNumPIDs; ++iid) {
    memset(fTreeIDs[iid], 0, sizeof(Int_t) * fN[iid]);
  }
  memset(fN, 0, sizeof(Int_t) * fNumPIDs);
  fPreIid = -1;
  fPrePID.clear();
}

void SmartRefTableImpl::ExpandPIDs(Int_t numpids)
{
  //Expand the arrays of managed PID

  if (numpids <= fNumPIDs) return;

  // else add to internal tables
  Int_t oldNumPIDs = fNumPIDs;
  fNumPIDs  = numpids;

  // update fAllocSize
  Int_t *allocTemp = fAllocSize;
  fAllocSize = new Int_t[fNumPIDs];
  if (allocTemp)
    memcpy(fAllocSize, allocTemp, oldNumPIDs * sizeof(Int_t));
  memset(&fAllocSize[oldNumPIDs], 0, (fNumPIDs - oldNumPIDs) * sizeof(Int_t));
  delete [] allocTemp;

  // update fN 
  Int_t* nTemp = fN;
  fN = new Int_t[fNumPIDs];
  if (nTemp)
    memcpy(fN, nTemp, oldNumPIDs * sizeof(Int_t));
  memset(&fN[oldNumPIDs], 0, (fNumPIDs - oldNumPIDs) * sizeof(Int_t));
  delete nTemp;

  // update fTreeIDs
  Int_t** treeIDsTemp = fTreeIDs;
  fTreeIDs = new Int_t *[fNumPIDs];
  if (treeIDsTemp)
    memcpy(fTreeIDs, treeIDsTemp, oldNumPIDs * sizeof(Int_t *));
  memset(&fTreeIDs[oldNumPIDs], 0, (fNumPIDs - oldNumPIDs) * sizeof(Int_t*));
}


Int_t SmartRefTableImpl::ExpandForIID(Int_t iid, Int_t newsize)
{
  // Expand fTreeIDs to newsize for internel ProcessID index iid

  if (newsize < 0)
    return newsize;
  if (newsize != fAllocSize[iid]) {
    Int_t *temp = fTreeIDs[iid];
    if (newsize != 0) {
      fTreeIDs[iid] = new Int_t[newsize];
      if (newsize < fAllocSize[iid]) {
            // contract, just copy old values
        memcpy(fTreeIDs[iid], temp, newsize * sizeof(Int_t));
      }
      else {
            // expand, copy old values and set new values to 0
        memcpy(fTreeIDs[iid], temp, fAllocSize[iid] * sizeof(Int_t));
        memset(&fTreeIDs[iid][fAllocSize[iid]], 0, (newsize - fAllocSize[iid]) * sizeof(Int_t));
      }
    }
    else {
      fTreeIDs[iid] = 0;
    }
    if (fAllocSize[iid])
      delete [] temp;
    fAllocSize[iid] = newsize;
  }
  return newsize;
}

Int_t SmartRefTableImpl::FindPIDGUID(const std::string& guid) const
{
  // Get fProcessGUIDs' index of the TProcessID with GUID guid
  std::vector<std::string>::const_iterator posPID = std::find(fProcessGUIDs.begin(), fProcessGUIDs.end(), guid);
  if (posPID == fProcessGUIDs.end()) return -1;
  return posPID - fProcessGUIDs.begin();
}

Int_t SmartRefTableImpl::GetBranchID(Int_t uid, const TProcessID* pid)
{
  Int_t iid = GetInternalIdxForPID(pid->GetTitle());

  uid = uid & 0xFFFFFF;
  if (uid < 0 || uid >= fN[iid]) return -1;
  return ( fTreeIDs[iid][uid] >> 24 ) - 1;
}

Int_t SmartRefTableImpl::GetTreeID(Int_t uid, const TProcessID* pid)
{
  Int_t iid = GetInternalIdxForPID(pid->GetTitle());

  uid = uid & 0xFFFFFF;
  if (uid < 0 || uid >= fN[iid]) return -1;
  return ( fTreeIDs[iid][uid] & 0xFFFFFF ) - 1;
}

Int_t SmartRefTableImpl::GetInternalIdxForPID(const std::string& guid) const
{
  // Get the index for fProcessIDs
  // Uses fMapPIDtoInternal and the pid's GUID / fProcessGUID

  return const_cast <SmartRefTable*>(this)->AddInternalIdxForPID(guid);
}

void SmartRefTableImpl::ReadMetaData(JM::TreeMetaData* metadata, Int_t treeid)
{
  // Read TreeMetaData

  std::vector<std::vector<Short_t> > bids = metadata ->GetBranchIDs();
  std::vector<std::vector<Int_t> > uids = metadata->GetUniqueIDs();
  std::vector<std::string> guids = metadata ->GetGUIDs();

  std::vector<std::vector<Int_t> >::const_iterator it_uids;
  std::vector<std::vector<Short_t> >::const_iterator it_bids;
  std::vector<Int_t>::const_iterator it_uid;
  std::vector<Short_t>::const_iterator it_bid;
  std::vector<std::string>::const_iterator it_guids;
  for (it_uids = uids.begin(), it_bids = bids.begin(), it_guids = guids.begin(); it_guids != guids.end(); ++it_uids, ++it_bids, ++it_guids) {
    for (it_uid = it_uids->begin(), it_bid = it_bids->begin(); it_uid != it_uids->end(); ++it_uid, ++it_bid) {
      Add(*it_guids, *it_uid, *it_bid, treeid);
    }
  }
}
