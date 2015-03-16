#ifndef ROOTIOTOOLS_TREELOOPER_H
#define ROOTIOTOOLS_TREELOOPER_H

#include "TObject.h"
#include <string>
#include <vector>
#include <map>

class TTree;
class TFile;

class TreeMerger {
    
    public:
        TreeMerger(const std::string& path, const std::string& objName);
        ~TreeMerger();

        bool next();
        void newTree(TTree* tree, const std::vector<Long64_t>& iBreakPoints);
        void writeTree();
        const std::vector<Long64_t>& getBreakPoints() const { return m_breakPoints; }

    private:
        TTree* m_oTree;
        TTree* m_iTree;
        void* m_addr;
        int m_idx;
        int m_entries;
        std::vector<Long64_t> m_breakPoints;

};

class TreeLooper {

    public:
        typedef std::map<std::string, TreeMerger*> TreeMap; // Key: data path; Value: TreeMerger
        typedef std::map<std::string, std::string> PathMap;
        typedef std::map<std::string, std::vector<Long64_t> > BreakPointsMap;
        TreeLooper(const PathMap& dataPathMap, TFile* file);
        ~TreeLooper();
        
        bool next();
        void finalize();
        void newInputFile(const std::string& value);
        BreakPointsMap& getBreakPoints() { return m_breakPoints; }

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
