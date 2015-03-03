#ifndef IMERGER_H
#define IMERGER_H

class IMerger {

    public:
        typedef std::vector<std::string> StringVector;
        IMerger() {}
        virtual ~IMerger() {}
        
        virtual void merge(TObject*& obj, std::string& path, std::string& name) =0;
        void setInputFile(const StringVector& value) { m_inputFiles = value; }

    private:
        StringVector  m_inputFiles;

};


#endif
