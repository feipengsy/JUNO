#include "UniqueIDTable.h"

JM::UniqueIDTable::~UniqueIDTable()
{
    for (TableMap::iterator it = m_tables.begin(); it != m_tables.end(); ++it) {
        delete it->second;
    }
}

void JM::UniqueIDTable::AddTable(const std::string& treename,
                                 const JM::TablePerTree::GUIDVector& guid,
                                 const JM::TablePerTree::UIDVector& uid,
                                 const JM::TablePerTree::BIDVector& bid)
{
    TablePerTree* table = new TablePerTree();
    table->SetGUIDs(guid);
    table->SetIDs(uid, bid);
    m_tables.insert(std::make_pair(treename, table));
}

void JM::UniqueIDTable::MergeTable(const UniqueIDTable& table)
{
    for (TableMap::iterator it = table.m_tables.begin(); it != table.m_tables.end(); ++it) {
        if (m_tables.find(it->first) == m_tables.end()) {
            TablePerTree* newTPR = new TablePerTree(*it->second);
            m_tables.insert(TableMap::value_type(it->first, newTPR));
        }
        else {
            m_tables[it->first]->MergeTable(*it->second);
        }
    }
}
