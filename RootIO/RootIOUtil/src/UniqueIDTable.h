#ifndef ROOTIOUTIL_UNIQUEIDTABLE_H
#define ROOTIOUTIL_UNIQUEIDTABLE_H

#include "TObject.h"

#include <vector>
#include <map>
#include <string>

namespace JM {

class TablePerTree {

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

        void SetIDs(const UIDVector& uids, const BIDVector& bids);
        void SetGUIDs(const GUIDVector& guids);

    private:
        UIDVector m_UniqueIDs;
        BIDVector m_BranchIDs;
        GUIDVector m_GUIDs;
};

class UniqueIDTable {

    public:
        typedef std::map<std::string, TablePerTree*> TableMap;

        UniqueIDTable();
        ~UniqueIDTable();

        void AddTable(const std::string& treename, 
                      const TablePerTree::GUIDVector& guid, 
                      const TablePerTree::UIDVector& uid, 
                      const TablePerTree::BIDVector& bid);

        TableMap& GetTable() { return m_tables; }
        const TableMap& GetTable() const { return m_tables; } 

    private:
        TableMap m_tables;

};

}  // namespace JM

#endif
