#include "UniqueIDTable.h"

JM::UniqueIDTable::~UniqueIDTable()
{
    for (TableMap::iterator it = m_tables.begin(); it != m_tables.end(); ++it) {
        delete it->second;
    }
}

void JM::UniqueIDTalbe::AddTable(const std::string& treename,
                                 const JM::TablePerTree::GUIDVector& guid,
                                 const JM::TablePerTree::UIDVector& uid,
                                 const JM::TablePerTree::BIDVector& bid)
{
    TablePerTree* table = new TablePerTree();
    table->SetGUID(guid);
    table->SetIDs(uid, bid);
    m_tables.insert(std::make_pair(treename, table));
}
