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
        void newTree(TTree* tree, const std::vector<int>& iBreakPoints);
        void writeTree();
        const std::vector<int>& getBreakPoints() const { return m_breakPoints; }

    private:
        TTree* m_oTree;
        TTree* m_iTree;
        void* m_addr;
        int m_idx;
        int m_entries;
        std::vector<int> m_breakPoints;

};

class TreeLooper {

    public:
        typedef std:map<std::string, TreeMerger*> TreeMap; // Key: data path; Value: TreeMerger
        typedef std::map<std::string, std::string> PathMap;
        typedef std::map<std::string, std::vector<int> > BreakPointsMap;
        TreeLooper(const PathMap& dataPathMap, TFile* file);
        ~TreeLooper();
        
        bool next();
        void finalize();
        void newInputFile(const std::string& value);

    private:
        TFile*          m_outputFile;
        TFile*          m_inputFile;
        // For event data trees
        TreeMap         m_treeMap;
        // For EvtNavigator tree
        TTree*          m_iNavTree;
        TTree*          m_oNavTree;
        void*           m_addr;
        int             m_idx;
        int             m_entries;
        BreakPointsMap  m_breakPoints;
};

#endif
