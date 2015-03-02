#ifndef ROOTIOTOOLS_TREELOOPER_H
#define ROOTIOTOOLS_TREELOOPER_H

#include <string>
#include <vector>

class TTree;

class TreeMerger {
    
    public:
        TreeMerger(const std::string& path, const std::string& objName);
        ~TreeMerger();

        bool next();
        void newTree(TTree* tree);

    private:
        TTree* m_oTree;
        TTree* m_iTree;
        void* m_addr;
        int m_idx;
        int m_entries;

};

class TreeLooper {

    public:
        typedef std:map<std::string, TreeMerger*> TreeMap; // Key: data path; Value: TreeMerger
        typedef std::map<std::string, std::pair<std::string, std::string> > PathMap;
        TreeLooper(const PathMap& dataPathMap);
        ~TreeLooper();
        
        void initialize();
        bool next();
        void finalize();
        void newInputFile(const std::string& value);

    private:
        TreeMap  m_treeMap;

};

#endif
