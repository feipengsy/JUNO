#ifndef ROOTIOTOOLS_TREELOOPER_H
#define ROOTIOTOOLS_TREELOOPER_H

#include <string>
#include <vector>

class TreeLooper {

    public:
        typedef std::vector<std::string> StringVector
        TreeLooper(const StringVector& dataPathList);
        ~TreeLooper();
        
        void initialize();
        bool next();
        void finalize();

    private:
        StringVector  m_pathList;

};

#endif
