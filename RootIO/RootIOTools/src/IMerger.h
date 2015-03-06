#ifndef IMERGER_H
#define IMERGER_H

#include <vector>
#include <string>

class TObject;

class IMerger {

    public:
        typedef std::vector<std::string> StringVector;
        IMerger() {}
        virtual ~IMerger() {}
        
        virtual void merge(TObject*& obj, std::string& path, std::string& name) =0;
        void setInputFile(const StringVector& value) { m_inputFiles = value; }

    protected:
        StringVector  m_inputFiles;

};


#endif
