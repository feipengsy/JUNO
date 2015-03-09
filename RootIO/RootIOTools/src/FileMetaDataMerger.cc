#include "FileMetaDataMerger.h"
#include "RootIOUtil/RootFileReader.h"
#include "RootIOUtil/FileMetaData.h"
#include "TFile.h"

FileMetaDataMerger::FileMetaDataMerger(std::map<std::string, std::vector<int> >* breakPoints)
       : IMerger(), m_breakPoints(breakPoints)
{
}

FileMetaDataMerger::~FileMetaDataMerger()
{
}

void FileMetaDataMerger::merge(TObject*& obj, std::string& path, std::string& name)
{
    TFile* file = new TFile(m_inputFiles[0].c_str(), "read");
    JM::FileMetaData* ifmd = RootFileReader::GetFileMetaData(file);
    JM::FileMetaData* ofmd = new JM::FileMetaData(*ifmd);
    ofmd->SetBreakPoints(*m_breakPoints);
    obj = ofmd;
    delete ifmd;
    file->Close();
    delete file;
    path = "Meta";
    name = "FileMetaData";
}
