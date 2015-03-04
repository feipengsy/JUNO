#include "UniqueIDTableMerger.h"

UniqueIDTableMerger::UniqueIDTableMerger()
    : IMerger()
{
}

UniqueIDTableMerger::~UniqueIDTableMerger()
{
}

void UniqueIDTableMerger::merge(TObject*& obj, std::string& path, std::string& name)
{
    JM::UniqueIDTable* oTable = new JM::UniqueIDTable;
    IMerger::StringVector::iterator it, end = m_inputFiles.end();
    for (it = m_inputFiles.begin(); it != end; ++it) {
        JM::UniqueIDTable* iTable = RootFileReader::GetUniqueIDTable(*it);
        oTable->MergeTable(iTable);
        delete iTable;
    }
    obj = oTable;
    path = "Meta";
    name = "UniqueIDTable";
}
