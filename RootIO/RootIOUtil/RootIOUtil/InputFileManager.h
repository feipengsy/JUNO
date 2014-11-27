/*  Class InputFileManager
 *  
 *    InputFileManager manages a list of InputFileHandle. InputFileManager 
 *  provides every registered input file an id and InputElementKeeper will
 *  manage the opening and closing of input files with it.
 *  
 *  Class InputFileHandle
 *
 *    InputFileHandle holds one paticular input file. It counts the active
 *  tree number of this file. When the count gets to 0, the file will be
 *  closed, in case of memory leaking.
 */
#ifndef ROOTIOUTIL_INPUTFILEMANAGER_H
#define ROOTIOUTIL_INPUTFILEMANAGER_H

#include <string>
#include <map>
#include <vector>

class TFile;
class TTree;

class InputFileHandle {
  
public:

    InputFileHandle(const std::string& filename, int fileid);
    ~InputFileHandle();
    
    // Close the file
    void close();
    
    // Update the pointer to TFile when file is re-opened
    void UpdateFile(TFile* file);

    // Get the pointer to TFile
    TFile* GetFile() { return m_file; }

    // Add number of active trees by one
    void AddTreeRef();

    // Minus number of active trees by one
    void DecTreeRef();

    // Set the nav tree reference flag
    void SetNavTreeRef();

    // Reset the nav tree reference flag
    void ResetNavTreeRef();

    // Get the status of the file
    bool GetStatus() const { return m_status; }

    // Reset the tree info when the file is re-opened
    void SetTreeInfo(std::map<int,std::string>& treeinfo);

    // Get the name of the file
    std::string& GetFileName() { return m_name; }

    // Get info of the trees holded by this file
    std::map<int,std::string>& GetTreeInfo() { return m_treeInfo; }
 
  private:
    std::string m_name;  // Name of the file
    bool m_status;  // Status of the file
    bool m_navTreeRefFlag;
    std::map<int,std::string> m_treeInfo;
    int m_activeTrees;
    TFile* m_file;
    int m_fileID;

};

class InputFileManager {
public:

    InputFileManager() {}
    ~InputFileManager();
    
    // Add a input file
    int AddFile(std::string& filename);

    // Get the file id of the given file, -1 if not found
    int FindFile(std::string& filename);
    
    // Reset the tree info when one file is re-opened
    void SetTreeInfo(int fileid, std::map<int,std::string>& treeinfo);
    
    // Add number of active trees by one
    void AddTreeRef(int fileid);
  
    // Minus number of active trees by one
    void DecTreeRef(int fileid);

    // Set the nav tree reference flag
    void SetNavTreeRef(int fileid);

    // Reset the nav tree reference flag
    void ResetNavTreeRef(int fileid);

    // Get the pointer to TFile
    TFile* GetFile(int fileid);

    // Check status of file
    bool CheckFileStatus(int fileid) const;

    // Update the pointer to TFile when file is re-opened
    void UpdateFile(int fileid, TFile* file);

    // Get the name of file
    std::string& GetFileName(int fileid);

    // Get info of trees of one file
    std::map<int,std::string>& GetTreeInfo(int fileid);

  private:
    std::vector<InputFileHandle*> m_files;

    // Close one file
    void close(int fileid);
};

#endif
