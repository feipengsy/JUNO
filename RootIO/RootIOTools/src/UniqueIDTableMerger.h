#ifndef UNIQUEIDTABLEMERGER_H
#define UNIQUEIDTABLEMERGER_H

class UniqueIDTableMerger {

    public:
        UniqueIDTableMerger();
        ~UniqueIDTableMerger();

        void merge(TObject*& obj, std::string& path, std::string& name);

};

#endif
