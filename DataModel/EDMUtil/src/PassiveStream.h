#ifndef PASSIVE_STREAM_H
#define PASSIVE_STREAM_H 0

#include <vector>
#include "TObject.h"

class TTree;

class PassiveStream {

    struct TreeHandle {
        TreeHandle(int fileID);
        int fileID;
        TTree* tree;
    };

    public:
        PassiveStream();
        ~PassiveStream();

        int AddTree(int fileid);
        void UpdateTree(int treeIndex, TTree* tree);
        bool ReadObject(int treeIndex, Long64_t entry);

    private:
        int  m_lastReadTreeIndex;
        std::vector<TreeHandle*> m_trees;
};

#endif
