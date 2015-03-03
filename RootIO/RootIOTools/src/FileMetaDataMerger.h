#ifndef FILEMETADATAMERGER_H
#define FILEMETADATAMERGER_H

class FileMetaDataMerger : public IMerger {

    public:
        FileMetaDataMerger();
        ~FileMetaDataMerger();

        void merge(TObject*& obj, std::string& path, std::string& name);

};

#endif
