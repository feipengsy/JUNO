#include "FileMetaDataMerger.h"

FileMetaDataMerger::FileMetaDataMerger(const std::map<std::string, std::vector<int> >* breakPoints)
       : IMerger(), m_breakPoints(breakPoints)
{
}

FileMetaDataMerger::~FileMetaDataMerger()
{
}

void FileMetaDataMerger::merge(TObject*& obj, std::string& path, std::string& name)
{
    JM::FileMetaData* ifmd = RootFileReader::GetFileMetaData(m_inputFiles[0]);
    JM::FileMetaData* ofmd = new JM::FileMetaData(ifmd);
    ofmd->SetBreakPoints(*m_breakPoints);
    obj = ofmd;
    delete ifmd;
    path = "Meta";
    name = "FileMetaData";
}
