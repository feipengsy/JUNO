#ifndef INPUT_FILE_MANAGER_H
#define INPUT_FILE_MANAGER_H 0

#include <string>
#include <map>
#include <vector>

class TFile;
class TTree;

class InputFileHandle {
  
public:

    InputFileHandle(const std::string& filename, int fileid);
    ~InputFileHandle();
    // Update the pointer to TFile when file is re-opened
    void UpdateFile(TFile* file);
    // Get the pointer to TFile
    TFile* GetFile() { return m_file; }
    // Add active trees count by one
    void AddTreeRef();
    // Minus active trees count by one
    void DecTreeRef();
    // Add active objects(SmartRef) count by one
    void AddObjRef();
    // Minus active objects(SmartRef) count by one
    void DecObjRef();
    // Get the status(opened or closed) of the file
    bool GetStatus() const { return m_status; }
    // Add one tree info holded by this file
    void AddTreeInfo(int treeid, const std::string& treename);
    // Get the name of the file
    const std::string& GetFileName() { return m_name; }
    // Get info of the trees holded by this file
    const std::map<int,std::string>& GetTreeInfo() { return m_treeInfo; }

  private:
    // Close the file
    void Close();
    // Check if it's time to close the file
    void CheckClose();

 
  private:
    std::string m_name;  // Name of the file
    bool m_status;  // Status of the file
    std::map<int,std::string> m_treeInfo;
    int m_activeTrees;
    int m_activeEntries;
    int m_fileID;
    TFile* m_file;

};

class InputFileManager {
public:

    InputFileManager() {}
    ~InputFileManager();
    // Add a input file
    int AddFile(const std::string& filename);
    // Get the file id of the given file, -1 if not found
    int FindFile(const std::string& filename);
    // Reset the tree info when one file is re-opened
    void AddTreeInfo(int fileid, int treeid, const std::string& treename);
    // Add number of active trees by one
    void AddTreeRef(int fileid);
    // Minus number of active trees by one
    void DecTreeRef(int fileid);
    void AddObjRef(int fileid);
    void DecObjRef(int fileid);
    // Get the pointer to TFile
    TFile* GetFile(int fileid);
    // Check status of file
    bool CheckFileStatus(int fileid) const;
    // Update the pointer to TFile when file is re-opened
    void UpdateFile(int fileid, TFile* file);
    // Get the name of file
    const std::string& GetFileName(int fileid);
    // Get info of trees of one file
    const std::map<int,std::string>& GetTreeInfo(int fileid);

  private:
    std::vector<InputFileHandle*> m_files;
};

#endif
