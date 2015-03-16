#ifndef FILEMETADATAMERGER_H
#define FILEMETADATAMERGER_H

#include "IMerger.h"
#include "TObject.h"
#include <vector>
#include <map>
#include <string>

class FileMetaDataMerger : public IMerger {

    public:
        FileMetaDataMerger(std::map<std::string, std::vector<Long64_t> >* breakPoints);
        ~FileMetaDataMerger();

        void merge(TObject*& obj, std::string& path, std::string& name);

    private:
        std::map<std::string, std::vector<Long64_t> >*  m_breakPoints;

};

#endif
