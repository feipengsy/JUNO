#ifndef ROOTIOTOOLS_TREELOOPER_H
#define ROOTIOTOOLS_TREELOOPER_H

#include <string>
#include <vector>

class TTree;

class TreeMerger {
    
    public:
        TreeMerger(const std::string headerName, const std::string eventName);
        ~TreeMerger();

        bool next();
        void newTree(TTree* header, TTree* event);

    private:
        TTree* m_oHeader, m_oEvent;
        TTree* m_iHeader, m_iEvent;
        int m_headerIndex, m_eventIndex;

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
