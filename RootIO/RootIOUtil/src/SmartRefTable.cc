#include "RootIOUtil/SmartRefTable.h"
#include "TTree.h"
#include "TBranch.h"
#include "TProcessID.h"

#include "RootIOUtil/TreeMetaData.h"
#include <algorithm>

SmartRefTable* SmartRefTable::fgSmartRefTable = 0;

SmartRefTable::SmartRefTable() : fTreeIDs(0), fPreIid(-1), fN(0), fNumPIDs(0), fAllocSize(0), fDefaultSize(10)
{
  // Default constructor of SmartRefTable
  
  fgSmartRefTable = this;
}

SmartRefTable::~SmartRefTable()
{
  // Destructor
  delete [] fAllocSize;
  delete [] fN;
  for (Int_t pid = 0; pid < fNumPIDs; ++pid) {
    delete [] fTreeIDs[pid];
  }
  delete [] fTreeIDs;
  if (fgSmartRefTable == this) fgSmartRefTable = 0;
}

void SmartRefTable::Add(const std::string& guid, Int_t uid, Int_t bid, Int_t tid)
{
  // Add a referenced object to SmartRefTable
  // Called by SmartRefTable::ReadMetaData()

  // get or generate a iid for a TProcessID title 
  Int_t iid = GetInternalIdxForPID( guid );

  Int_t newsize = 0;
  uid = uid & 0xffffff;
  // expand fTreeIDs if necessary
  if (uid >= fAllocSize[iid]) {
    newsize = uid + uid / 2;
    if (newsize < fDefaultSize)
      newsize = fDefaultSize;
    newsize = ExpandForIID(iid, newsize);
  }
  if (newsize < 0) {
    // fail to expand or fTreeIDs
    return;
  }
  fTreeIDs[iid][uid] = tid + 1;
  if (uid >= fN[iid]) fN[iid] = uid + 1;
}

Int_t SmartRefTable::AddInternalIdxForPID(const std::string& guid)
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

void SmartRefTable::Clear()
{
  // Clear the table

  for (Int_t iid = 0; iid < fNumPIDs; ++iid) {
    memset(fTreeIDs[iid], 0, sizeof(Int_t) * fN[iid]);
  }
  memset(fN, 0, sizeof(Int_t) * fNumPIDs);
  fPreIid = -1;
  fPrePID.clear();
}

void SmartRefTable::ExpandPIDs(Int_t numpids)
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


Int_t SmartRefTable::ExpandForIID(Int_t iid, Int_t newsize)
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

Int_t SmartRefTable::FindPIDGUID(const std::string& guid) const
{
  // Get fProcessGUIDs' index of the TProcessID with GUID guid
  std::vector<std::string>::const_iterator posPID = std::find(fProcessGUIDs.begin(), fProcessGUIDs.end(), guid);
  if (posPID == fProcessGUIDs.end()) return -1;
  return posPID - fProcessGUIDs.begin();
}

Int_t SmartRefTable::GetTreeID(Int_t uid, const TProcessID* pid)
{
  Int_t iid = GetInternalIdxForPID(pid->GetTitle());

  uid = uid & 0xFFFFFF;
  if (uid < 0 || uid >= fN[iid]) return -1;
  return fTreeIDs[iid][uid] - 1;
}

SmartRefTable* SmartRefTable::GetRefTable()
{
  //Static function returning the current SmartRefTable.
  
  return fgSmartRefTable;
}

Int_t SmartRefTable::GetInternalIdxForPID(const std::string& guid) const
{
  // Get the index for fProcessIDs
  // Uses fMapPIDtoInternal and the pid's GUID / fProcessGUID
  
  return const_cast <SmartRefTable*>(this)->AddInternalIdxForPID(guid);
}

void SmartRefTable::ReadMetaData(JM::TreeMetaData* metadata, Int_t treeid) 
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


