#ifndef ROOTIOTOOLS_TREEMERGER_H
#define ROOTIOTOOLS_TREEMERGER_H

#include <string>
#include <vector>

class TreeMerger {

    public:
        typedef std::vector<std::string> StringVector
        TreeMerger(const StringVector& dataPathList);
        ~TreeMerger();
        
        void initialize();
        bool next();
        void finalize();

    private:
        StringVector  m_pathList;

};

#endif
