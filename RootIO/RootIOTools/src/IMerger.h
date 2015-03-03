#ifndef IMERGER_H
#define IMERGER_H

class IMerger {

    public:
        IMerger() {}
        virtual ~IMerger() {}
        
        virtual void merge() =0;
        void setInputFile(const std::vector<std::string>& value) { m_inputFiles = value; }

    private:
        std::vector<std::string>  m_inputFiles;

};


#endif
