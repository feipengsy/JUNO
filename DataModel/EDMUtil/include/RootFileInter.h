#ifndef ROOT_FILE_INTER_H
#define ROOT_FILE_INTER_H 0

#include <string>

class TFile;
class TTree;
class TObject;

namespace JM {
    class FileMetaData;
    class UniqueIDTable;
}

class RootFileInter {

    public:
        // Static function to open a file
        static TFile* OpenFile(const std::string& filename);
        // Static function to get the tree holding event data
        static TTree* GetTree(TFile* file, const std::string& treename);
        // Get the FileMetaData of a TFile
        static JM::FileMetaData* GetFileMetaData(TFile* file);
        // Get the UniqueIDTable of a TFile
        static JM::UniqueIDTable* GetUniqueIDTable(TFile* file);
        // Get addtional TObject of input file
        static TObject* ReadObject(const std::string& filename, const std::string& objName);
        static TObject* ReadObject(TFile* file, const std::string& objName);
};

#endif
