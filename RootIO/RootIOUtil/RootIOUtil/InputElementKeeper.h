/*  Class InputElementKeeper
 *  
 *  InputElementKeeper is a singleton class shared by
 *  all input streams of EvtNavigator
 *
 *  InputElementKeeper holds SmartRefTable, InputFlieManager
 *  and InputTreeManager and provides interfaces of them.
 *  
 */

#ifndef ROOTIOUTIL_INPUTELEMENTKEEPER_H
#define ROOTIOUTIL_INPUTELEMENTKEEPER_H

#include "TObject.h"

#include <map>
#include <string>
#include <vector>

class SmartRefTable;
class InputTreeManager;
class InputFileManager;
class TBranch;
class TTree;
class TFile;
class TProcessID;

namespace JM {
  
  class FileMetaData;
  class TreeMetaData;
  
}


class InputElementKeeper {

    public:

        typedef std::map<std::string, std::vector<int> > String2FileIDs;

        // Destructor
        ~InputElementKeeper();
        // Static function to get the class
        static InputElementKeeper* GetInputElementKeeper(bool create = true);
        // Add reference count of this object by 1
        void AddRef();
        // Minus reference count of this object by 1
        void DecRef();
        // Set nav tree reference of one file
        void SetNavTreeRef(int fileid);
        // Reset nav tree refernence of one file
        void ResetNavTreeRef(int fileid);
        // Given a SmartRef, add active entries of the parent tree by 1
        void AddObjRef(Int_t uid, const TProcessID* pid);
        // Add active trees number of one file by 1
        void AddTreeRef(int fileid);
        // Clear the SmartRefTable, reclaim memory
        void ClearTable();
        // Minus active trees number of one file by 1, if goes to 0, close the file
        void DecTreeRef(int fileid);
        // Called by SmartRef::clear()
        void DelObj(Int_t uid, TProcessID* pid, Long64_t entry);
        // Get the TTree* owning EvtNavigators of a file
        bool GetNavTree(int fileid, TTree*& tree);
        // Get file name
        std::string& GetFileName(int fileid);
        // Get the pointer of the TFile
        TFile* GetFile(int fileid);
        // Open a certain file
        void OpenFile(int fileid);
        // Register a new input file into this keeper
        int RegisterFile(const std::string& filename, const std::vector<JM::TreeMetaData*>& trees);
        // Reigster the map of data path and input file list
        void RegisterPathMap(const String2FileIDs& pathmap);
        // Register the map of TProcessID UUID and input file list
        void RegisterUUIDMap(const String2FileIDs& uuidmap);
        // Check file status of a certain input file
        bool CheckFileStatus(int fileid) const;
        // Given a SmartRef, get the parent branch
        TBranch* GetBranch(Int_t uid, const TProcessID* pid, Int_t branchID = -1);
        // Given a path, get the input file list of it
        std::vector<int> GetFileList(const std::string& path);

    private:    
        // Singleton class, private constructor
        InputElementKeeper();
        void RegisterFileMap(const String2FileIDs& value, const std::string& type);
        // Load meta data of one file into SmartRefTable
        void LoadUniqueID(int fileid, const std::string& filename);

    private:
        SmartRefTable*              m_table;
        InputTreeManager*           m_treeMgr;
        InputFileManager*           m_fileMgr;
        String2FileIDs              m_path2FileList;
        String2FileIDs              m_uuid2FileList;
        // Singleton class
        int                         m_refCount;   // Reference count
        static InputElementKeeper*  m_keeper;     // Current InputElementKeeper
};

#endif
