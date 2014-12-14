/*  Class RootFileReader
 *
 *  RootFileReader is designed to open root files and
 *  to read the meta datas in it.
 *
 */
#ifndef ROOTIOUTIL_ROOTFILEREADER_H
#define ROOTIOUTIL_ROOTFILEREADER_H

#include <string>
#include <map>
#include <vector>

class NavTreeList;

namespace JM {

    class FileMetaData;
    class UniqueIDTable;

}

class RootFileReader { 

public:

    ~RootFileReader() {}

    // Open a registered input file again, called by InputElementKeeper
    static bool ReOpen(const std::string& filename, TFile*& file, const std::map<int,std::string>& treeinfo, std::vector<TTree*>& trees);

    // Open a input file and register it to InputElementKeeper
    static bool ReadFiles(const std::vector<std::string>& fileList, NavTreeList* navs, std::vector<std::string>& path, std::vector<std::string>& eventName);

    // Static function to open a file
    static TFile* OpenFile(const std::string& filename);

    // Static function to get the tree owning EvtNavigator
    static TTree* GetNavTree(TFile* file);

    // Given FileMetaDatas and file ids, initialize NavTreeList
    static bool GetNavTreeList(std::map<int,JM::FileMetaData*>& fmetadatas, NavTreeList* navs, std::vector<std::string>& path, std::vector<std::string>& eventName);

    // Get the FileMetaData of a TFile
    static JM::FileMetaData* GetFileMetaData(TFile* file);

    // Get the UniqueIDTable of a TFile
    static JM::UniqueIDTable* GetUniqueIDTable(TFile* file);

    // Get addtional TObject of input file(s)
    static TObject* GetUserData(const std::vector<int>& fileList, const std::string& name);

private:
    RootFileReader() {}
    static TObject* ReadObject(TDirectory* dir, const std::string& name);

};

#endif
