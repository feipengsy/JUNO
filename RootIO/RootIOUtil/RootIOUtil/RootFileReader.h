/*  Class RootFileReader
 *
 *  RootFileReader is designed to open root files and
 *  to read the meta datas in it.
 *
 */
#ifndef ROOTIOUTIL_ROOTFILEREADER_H
#define ROOTIOUTIL_ROOTFILEREADER_H

#include "TFile.h"
#include "TTree.h"

#include <string>
#include <map>
#include <vector>

class NavTreeList;
class InputElementKeeper;

namespace JM {

    class FileMetaData;
    class UniqueIDTable;

}

class RootFileReader { 

public:

    RootFileReader(InputElementKeeper* keeper) : m_keeper(keeper), m_name("RootFileReader") {}
    ~RootFileReader() {}

    // Add a input file
    bool AddFile(const std::string& filename);

    // Open a registered input file again, called by InputElementKeeper
    static bool ReOpen(const std::string& filename, TFile*& file, const std::map<int,std::string>& treeinfo, std::vector<TTree*>& trees);

    // Open a input file and register it to InputElementKeeper
    bool ReadFiles(NavTreeList* navs, std::vector<std::string>& path, std::vector<std::string>& eventName);

    // Static function to open a file
    static TFile* OpenFile(const std::string& filename);

    // Static function to get the tree owning EvtNavigator
    static TTree* GetNavTree(TFile* file);

    // Given FileMetaDatas and file ids, initialize NavTreeList
    bool GetNavTreeList(std::map<int,JM::FileMetaData*>& fmetadatas, NavTreeList* navs, std::vector<std::string>& path, std::vector<std::string>& eventName);

    // Get the FileMetaData of a TFile
    static JM::FileMetaData* GetFileMetaData(TFile* file);

    // Get the UniqueIDTable of a TFile
    static JM::UniqueIDTable* GetUniqueIDTable(TFile* file);

    // Get addtional TObject of input file(s)
    TObject* GetUserData(const std::vector<int>& fileList, const std::string& name);

    // For SniperLog
    const std::string& objName() { return m_name; }

private:
    std::vector<std::string> m_filenames;
    InputElementKeeper* m_keeper;
    std::string m_name;

private:
    TObject* ReadObject(TDirectory* dir, const std::string& name);

};

#endif
