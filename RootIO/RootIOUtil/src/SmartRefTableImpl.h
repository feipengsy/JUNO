#ifndef ROOTIOUTIL_SMARTREFTABLEIMPL_H
#define ROOTIOUTIL_SMARTREFTABLEIMPL_H

class SmartRefTableImpl {

    public:

        SmartRefTableImpl();
        ~SmartRefTableImpl();
        // Register a referenced object into this table
        void Add(const std::string& guid, Int_t uid, Int_t bid, Int_t tid);
        // Clear the table
        void Clear();
        // Clear the table and reclaim memory
        void Reset();
        // Given a SmartRef, get the id of the tree holding the object if refers to
        Int_t GetTreeID(Int_t uid, const TProcessID* pid);
        // Given a SmartRef, get the id of the branch holding the referenced object
        Int_t GetBranchID(Int_t uid, const TProcessID* pid);
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

        Int_t**                        m_TreeIDs;       // Array of tree ids 
        Int_t                          m_PreIid;        // Cached iid
        std::string                    m_PrePID;        // Cached TProcessID title
        std::vector<std::string>       m_ProcessGUIDs;  // Array of TProcessID titles
        Int_t*                         m_N;             // Max number of fTreeIDs[iid]
        Int_t                          m_NumPIDs;       // Number of known TProcessIDs
        Int_t*                         m_AllocSize;     // Length of fTreeIDs[iid]
        Int_t                          m_MinSize;       // Minimum allocating size of fTreeIDs
        Int_t                          m_MaxSize;       // Maximum allocating size of fTreeIDs
        Int_t                          m_fileID;        // File id of this table

};


#endif
