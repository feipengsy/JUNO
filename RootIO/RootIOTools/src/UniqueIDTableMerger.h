#ifndef UNIQUEIDTABLEMERGER_H
#define UNIQUEIDTABLEMERGER_H

class UniqueIDTableMerger {

    public:
        UniqueIDTableMerger(const std::map<std::string, std::vector<int> >* breakPoints);
        ~UniqueIDTableMerger();

        void merge(TObject*& obj, std::string& path, std::string& name);

    private:
        std::map<std::string, std::vector<int> >*  m_breakPoints;

};

#endif
