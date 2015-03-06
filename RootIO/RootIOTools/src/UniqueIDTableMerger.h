#ifndef UNIQUEIDTABLEMERGER_H
#define UNIQUEIDTABLEMERGER_H

#include "IMerger.h"
#include <string>

class TObject;

class UniqueIDTableMerger : public IMerger {

    public:
        UniqueIDTableMerger();
        ~UniqueIDTableMerger();

        void merge(TObject*& obj, std::string& path, std::string& name);

};

#endif
