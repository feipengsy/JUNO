#ifndef INPUT_ELEMENT_KEEPER_H
#define INPUT_ELEMENT_KEEPER_H 0

#include "TObject.h"

#include <map>
#include <string>
#include <vector>

class SmartRefTable;
class PassiveStream;
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

    private:
        struct TreeInfo {
            // TreeInfo records the information of one registered TTree
            TreeInfo(PassiveStream* ps, int index, int fileid, const std::vector<Long64_t>& bp);
            PassiveStream*  stream;       // The PassiveStream holds this tree
            int             streamIndex;  // Index of this tree in a PassiveStream
            int             fileID;       // The file holds this tree
            std::vector<Long64_t> breakPoints;
        };
        typedef std::map<std::string, std::vector<int> > String2FileIDs;
        typedef std::vector<TreeInfo*> TreeInfoList;
        typedef std::map<std::string, PassiveStream*> PSMap;

    public:
        // Notify() options
        enum {
            Read,
            Delete,
            New
        };
        // Destructor
        ~InputElementKeeper();
        // Static function to get the class
        static InputElementKeeper* GetInputElementKeeper(bool create = true);
        // Add reference count of this object by 1
        void AddRef();
        // Minus reference count of this object by 1
        void DecRef();
        // Add active trees number of one file by 1
        void AddTreeRef(int fileid);
        // Clear the SmartRefTable, reclaim memory
        void ClearTable(int fileid);
        // Minus active trees number of one file by 1, if goes to 0, close the file
        void DecTreeRef(int fileid);
        // Get the TTree* owning EvtNavigators of a file
        bool GetNavTree(int fileid, TTree*& tree);
        // Get file name
        const std::string& GetFileName(int fileid);
        // Get the pointer of the TFile
        TFile* GetFile(int fileid);
        // Register a new input file into this keeper
        int RegisterFile(const std::string& filename, const JM::FileMetaData* metadata);
        // Check file status of a certain input file
        bool CheckFileStatus(int fileid) const;
        // Get additional object of input file list
        bool GetObj(TObject*&, const std::string& objName);
        void Notify(int option, Int_t uid, const TProcessID* pid, Long64_t entry = -1);

    private:
        // Suppress default
        InputElementKeeper();
        // Load meta data of one file into SmartRefTable
        void LoadUniqueID(int fileid);
        // Given a SmartRef, add active entries of the parent file by 1
        void AddObjRef(Int_t uid, const TProcessID* pid);
        // Given a SmartRef, minus active entries of the parent file by 1
        void DecObjRef(Int_t uid, const TProcessID* pid);
        // Open a certain file
        void OpenFile(int fileid);
        // Lazy-load a object
        void ReadObject(Int_t uid, const TProcessID* pid, Long64_t entry);

    private:
        SmartRefTable*              m_table;
        InputFileManager*           m_fileMgr;
        PSMap                       m_psMap;
        TreeInfoList                m_treeInfoList;
        String2FileIDs              m_path2FileList;
        String2FileIDs              m_uuid2FileList;
        // Singleton
        int                         m_refCount;   // Reference count
        static InputElementKeeper*  m_keeper;     // Current InputElementKeeper
};

#endif
