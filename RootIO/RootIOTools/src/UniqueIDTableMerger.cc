#include "UniqueIDTableMerger.h"

UniqueIDTableMerger::UniqueIDTableMerger(const std::map<std::string, std::vector<int> >* breakPoints)
    : IMerger(), m_breakPoints(breakPoints)
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
        oTable->AddTable(iTable);
        delete iTable;
    }
    oTable->SetBreakPoints(*m_breakPoints);
    obj = oTable;
    path = "Meta";
    name = "UniqueIDTable";
}
