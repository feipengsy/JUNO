#ifndef SMARTREF_TABLE_IMPL_H
#define SMARTREF_TABLE_IMPL_H

#include "TObject.h"

#include <string>
#include <vector>

class TProcessID;

namespace JM {

    class TablePerTree;

}

class SmartRefTableImpl {

    public:

        SmartRefTableImpl(Int_t fileid);
        ~SmartRefTableImpl();
        // Register a referenced object into this table
        void Add(const std::string& guid, Int_t uid, Int_t bid, Int_t oid, Int_t tid);
        // Clear the table
        void Clear();
        // Clear the table and reclaim memory
        void Reset();
        // Given a SmartRef, get the id of the tree holding the object if refers to
        Int_t GetTreeID(Int_t uid, const TProcessID* pid);
        // Given a SmartRef, get the id of the branch holding the referenced object
        Int_t GetBranchID(Int_t uid, const TProcessID* pid);
        // Given a TProcessID, get the entry offset number(caused by merging)
        Int_t GetOffset(Int_t uid, const TProcessID* pid);
        // Get the file ID of this table
        Int_t GetFileID();
        // Read UniqueIDTable, register all referenced object it holds
        void ReadMetaData(JM::TablePerTree* table, Int_t treeid, const std::vector<Long64_t>& breakPoints);

    private:

        // Add one guid of TProcessID
        Int_t AddInternalIdxForPID(const std::string& guid, bool create);
        // Expand table for a new TProcessID
        Int_t ExpandForIID(Int_t iid, Int_t newsize);
        void  ExpandPIDs(Int_t numpids);
        Int_t FindPIDGUID(const std::string& guid) const;
        Int_t GetInternalIdxForPID(const std::string& guid, bool create) const;

    private:

        Int_t**                        m_TreeIDs;       // Array of tree ids 
        Int_t                          m_PreIid;        // Cached iid
        std::string                    m_PrePID;        // Cached TProcessID title
        std::vector<std::string>       m_ProcessGUIDs;  // Array of TProcessID titles
        Int_t*                         m_N;             // Max number of fTreeIDs[iid]
        Int_t                          m_NumPIDs;       // Number of known TProcessIDs
        Int_t*                         m_AllocSize;     // Length of fTreeIDs[iid]
        Int_t                          m_MinSize;       // Minimum allocating size of fTreeIDs
        Int_t                          m_MaxSize;       // Maximum allocating size of fTreeIDs
        Int_t                          m_FileID;        // File id of this table

};

#endif
