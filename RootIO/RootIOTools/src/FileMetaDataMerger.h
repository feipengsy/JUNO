#ifndef FILEMETADATAMERGER_H
#define FILEMETADATAMERGER_H

class FileMetaDataMerger : public IMerger {

    public:
        FileMetaDataMerger(const std::map<std::string, std::vector<int> >& breakPoints);
        ~FileMetaDataMerger();

        void merge(TObject*& obj, std::string& path, std::string& name);

};

#endif
