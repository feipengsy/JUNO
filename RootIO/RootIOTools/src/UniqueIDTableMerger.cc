#include "UniqueIDTableMerger.h"
#include "RootIOUtil/UniqueIDTable.h"
#include "RootIOUtil/RootFileReader.h"
#include "TFile.h"

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
        TFile* file = new TFile(it->c_str(), "recreate");
        JM::UniqueIDTable* iTable = RootFileReader::GetUniqueIDTable(file);
        oTable->MergeTable(*iTable);
        file->Close();
        delete file;
        delete iTable;
    }
    obj = oTable;
    path = "Meta";
    name = "UniqueIDTable";
}
