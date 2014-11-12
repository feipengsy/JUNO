#ifndef ROOTIOUTIL_INPUTTREEMANAGER_H
#define ROOTIOUTIL_INPUTTREEMANAGER_H

#include "TObject.h"

#include <vector>
#include <utility>

class TTree;

class InputTreeHandle {

  public:
    InputTreeHandle(int fileid) : m_fileID(fileid), m_activeEntries(0), m_entries(0), m_opened(false), m_tree(0) {}
    // Destructor dosen't have to delete m_tree
    ~InputTreeHandle() {}
    int AddRef() { return ++m_activeEntries; }
    bool LastObj(Long64_t entry);
    TTree* GetTree() { return m_tree; }
    void SetTree(TTree* tree);
    void Close() { m_opened = false; }
    int GetFileID() { return m_fileID; }

  private:
    int m_fileID;
    int m_activeEntries;
    int m_entries;
    bool m_opened;
    TTree* m_tree;

};

class InputTreeManager {

  public:
    InputTreeManager() {}
    ~InputTreeManager();
    void AddRef(int treeid);
    void DelObj(int treeid, Long64_t entry);
    int AddTree(int fileid);
    TTree* GetTree(int treeid);
    void ResetTree(int treeid, TTree* tree);
    int GetFileID(int treeid);

  private:
    std::vector<InputTreeHandle*> m_trees;

};

#endif
