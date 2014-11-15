/*  Class SmartRefTable
 *
 *  SmartRefTable holds the tree id and branch id of all 
 *  object referenced by SmartRef. During intializing
 *  of RootInputStream, information of referenced object
 *  will be added into this table.
 *
 *  When a SmartRef tries to load the object it refers
 *  to, it will query SmartRefTable to get the tree id
 *  and file id allocated by InputTreeManager and 
 *  InputFileManager.
 *  
 *  This class is inspired by TRefTable in ROOT. 
 */

#ifndef ROOTIOUTIL_SMARTREFTABLE_H
#define ROOTIOUTIL_SMARTREFTABLE_H

#include "TObject.h"

#include <vector>
#include <map>
#include <string>

class TTree;
class TBranch;
class TObjArray;
class TProcessID;

namespace JM {

    class TreeMetaData;

}

class SmartRefTable {
 //Table of referenced objects during an I/O operation

public:
    SmartRefTable();
    ~SmartRefTable();
    // Register a referenced object into this table
    void Add(const std::string& guid, Int_t uid, Int_t bid, Int_t tid);
    // Clear the table
    void Clear();
    // Given a SmartRef, get the id of the tree holding the object if refers to
    Int_t GetTreeID(Int_t uid, const TProcessID* pid);
    // Static method to get the current SmartRefTable
    static SmartRefTable* GetRefTable();
    // Read TreeMetadata, register all referenced object it holds
    void ReadMetaData(JM::TreeMetaData* metadata, Int_t treeid);

private:
    // Add one guid of TProcessID
    Int_t AddInternalIdxForPID(const std::string& guid);
    // Expand table for a new TProcessID
    Int_t ExpandForIID(Int_t iid, Int_t newsize);
    void  ExpandPIDs(Int_t numpids);
    Int_t FindPIDGUID(const std::string& guid) const;
    Int_t GetInternalIdxForPID(const std::string& guid) const;
    
private:
    Int_t**                     fTreeIDs;   // Array of tree ids 
    Int_t                       fPreIid;    // Cached iid
    std::string                 fPrePID;    // Cached TProcessID title
    std::vector<std::string>    fProcessGUIDs;  // Array of TProcessID titles
    Int_t*                      fN;         // Max number of fTreeIDs[iid]
    Int_t                       fNumPIDs;   // Number of known TProcessIDs
    Int_t*                      fAllocSize; // Length of fTreeIDs[iid]
    Int_t                       fDefaultSize;  // Default alloc size of fTreeIDs
    static SmartRefTable*       fgSmartRefTable; // Current SmartRefTable
};

#endif
