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
    TFile* file = new TFile(name, "recreate");
    JM::FileMetaData* ifmd = RootFileReader::GetFileMetaData(file);
    JM::FileMetaData* ofmd = new JM::FileMetaData(ifmd);
    ofmd->SetBreakPoints(*m_breakPoints);
    obj = ofmd;
    delete ifmd;
    file->Close();
    delete file;
    path = "Meta";
    name = "FileMetaData";
}
