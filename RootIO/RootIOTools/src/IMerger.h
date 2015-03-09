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
        void setOutputFile(const std::string& value) { m_outputFile = value; }

    protected:
        StringVector  m_inputFiles;
        std::string   m_outputFile;

};


#endif
