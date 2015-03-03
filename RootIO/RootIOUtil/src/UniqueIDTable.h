#ifndef ROOTIOUTIL_UNIQUEIDTABLE_H
#define ROOTIOUTIL_UNIQUEIDTABLE_H

#include "TObject.h"

#include <vector>
#include <map>
#include <string>

namespace JM {

class TablePerTree : public TObject {

    public:
        // Attribute type typedefs
        typedef std::vector<std::vector<Int_t> > UIDVector;
        typedef std::vector<std::vector<Short_t> > BIDVector;
        typedef std::vector<std::string> GUIDVector;

        TablePerTree() {}
        ~TablePerTree() {}

        // Setter and Getter functions
        BIDVector& GetBranchIDs() { return m_BranchIDs; }
        UIDVector& GetUniqueIDs() { return m_UniqueIDs; }
        GUIDVector& GetGUIDs() { return m_GUIDs; }
        const BIDVector& GetBranchIDs() const { return m_BranchIDs; }
        const UIDVector& GetUniqueIDs() const { return m_UniqueIDs; }
        const GUIDVector& GetGUIDs() const { return m_GUIDs; }

        void SetGUIDs(const GUIDVector& guids) { m_GUIDs = guids; }
        void SetIDs(const UIDVector& uids, const BIDVector& bids);

    private:
        UIDVector m_UniqueIDs;
        BIDVector m_BranchIDs;
        GUIDVector m_GUIDs;

    ClassDef(TablePerTree,1);
};

inline void TablePerTree::SetIDs(const UIDVector& uids, const BIDVector& bids)
{
    m_UniqueIDs = uids;
    m_BranchIDs = bids;
}

class UniqueIDTable : public TObject {

    public:
        typedef std::map<std::string, TablePerTree*> TableMap;

        UniqueIDTable() { }
        ~UniqueIDTable();

        void AddTable(const std::string& treename, 
                      const TablePerTree::GUIDVector& guid, 
                      const TablePerTree::UIDVector& uid, 
                      const TablePerTree::BIDVector& bid);
        void MergeTable(const UniqueIDTable& table);

        TableMap& GetTable() { return m_tables; }
        const TableMap& GetTable() const { return m_tables; } 

    private:
        TableMap m_tables;

    ClassDef(UniqueIDTable,1);

};

}  // namespace JM

#endif
