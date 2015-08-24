#ifndef ROOTIOUTIL_INPUTTREEMANAGER_H
#define ROOTIOUTIL_INPUTTREEMANAGER_H

#include "TObject.h"

#include <vector>
#include <utility>

class TTree;

class InputTreeHandle {

  public:
    InputTreeHandle(int fileid) : m_fileID(fileid), m_activeEntries(0), m_active(false), m_lastUID(-1), m_tree(0) {}
    // Destructor dosen't have to delete m_tree
    ~InputTreeHandle() {}
    bool LastObj(Int_t uid);
    int AddRef() { return ++m_activeEntries; }
    TTree* GetTree() { return m_tree; }
    void SetTree(TTree* tree);
    void Close() { m_active = false; }
    int GetFileID() { return m_fileID; }
    void SetBreakPoints(const std::vector<Long64_t>& value) { m_breakPoints = value; }
    void SetLastUID(Int_t value) { m_lastUID = value; }
    const std::vector<Long64_t>& GetBreakPoints() { return m_breakPoints; }
    Long64_t GetTreeOffset(int offsetid) { return m_breakPoints[offsetid]; }

  private:
    int m_fileID;
    int m_activeEntries;
    bool m_active;
    Int_t m_lastUID;
    std::vector<Long64_t> m_breakPoints;
    TTree* m_tree;

};

class InputTreeManager {

  public:
    InputTreeManager() {}
    ~InputTreeManager();
    void AddRef(int treeid);
    void DelObj(int treeid, Int_t uid);
    int AddTree(int fileid, const std::vector<Long64_t>& breakPoints);
    TTree* GetTree(int treeid);
    void ResetTree(int treeid, TTree* tree);
    void SetLastUID(int treeid, Int_t uid);
    int GetFileID(int treeid);
    Long64_t GetTreeOffset(int treeid, int offsetid);
    const std::vector<Long64_t>& GetBreakPoints(int treeid);

  private:
    std::vector<InputTreeHandle*> m_trees;

};

#endif
