#ifndef ROOTIOUTIL_ROOTOUTPUTFILEMANAGER_H
#define ROOTIOUTIL_ROOTOUTPUTFILEMANAGER_H

#include <map>
#include <vector>
#include <string>

#include "TGeoManager.h"

class TFile;
class TTree;

namespace JM {
    class FileMetaData;
    class TreeMetaData;
    class UniqueIDTable;
}

class RootOutputFileHandle {

public:
    typedef std::map<std::string, bool> PathMap;
    typedef std::vector<std::string> StringVector;
    typedef std::vector<TGeoManager*> GeoVector;
 
    RootOutputFileHandle(const std::string& filename, const std::map<std::string, int>& paths);
    ~RootOutputFileHandle();

    TFile* getFile();

    TTree* getNavTree();

    std::string getName();

    void setNavPath(const StringVector& paths);

    void setNavEventName(const StringVector& eventNames);

    void addTreeMetaData(JM::TreeMetaData* treemetadata);

    void addGeoManager(TGeoManager* geo);

    void addRef();

    void addUniqueIDTable(const std::string& treename,
                          const StringVector& guid,
                          const std::vector<std::vector<Int_t> >& uid,
                          const std::vector<std::vector<Short_t> >& bid);

    void addUUID(const StringVector& uuids);

    int decRef();

    bool isLastPath(const std::string& path);

    void setNavAddr(void* navAddr);

    void close();

    void writing(const std::string& path);

    bool hasPath(const std::string& path);

    void revise(const std::string& path, int priority);

private:
    TFile*              m_file;
    TTree*              m_navTree;
    JM::FileMetaData*   m_fileMetaData;
    JM::UniqueIDTable*  m_IDTable;
    StringVector        m_UUIDList;
    PathMap             m_paths;
    GeoVector           m_geos;
    int                 m_refCount;
    void*               m_navAddr;
};

class RootOutputFileManager {

public:
    typedef std::map<std::string, RootOutputFileHandle*> FileMap; // map<file name,file handle>
    
    ~RootOutputFileManager();
    // Singleton
    static RootOutputFileManager* get();
    // Create a new output file
    void new_file(const std::string& filename, const std::map<std::string, int>& pathMap);
    // Get file of given name
    RootOutputFileHandle* get_file_with_name(const std::string& filename);
    // Get file of given path, increase reference count of file
    // TODO One output stream can only go to one output file
    RootOutputFileHandle* get_file_with_path(const std::string& path);
    // Decrease file's ref count, really close when it hits zero.
    void close_file(const std::string& filename);
    // Revise the output file, set the path priority
    void reviseOutputFile(const std::string& filename, const std::string& path, int priority);

private:
    // Singleton
    // TODO Singleton within a task?
    RootOutputFileManager();

private:
    FileMap m_filemap;
    static RootOutputFileManager* m_fileMgr; // Pointer to self
};

#endif
