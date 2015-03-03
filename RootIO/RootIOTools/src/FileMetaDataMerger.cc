#include "FileMetaDataMerger.h"

FileMetaDataMerger::FileMetaDataMerger()
       : IMerger()
{
}

FileMetaDataMerger::~FileMetaDataMerger()
{
}

void FileMetaDataMerger::merge(TObject*& obj, std::string& path, std::string& name)
{
    JM::FileMetaData* ifmd = RootFileReader::GetFileMetaData(m_inputFiles[0]);
    obj = new JM::FileMetaData(ifmd);
    delete ifmd;
    path = "Meta";
    name = "FileMetaData";
}
