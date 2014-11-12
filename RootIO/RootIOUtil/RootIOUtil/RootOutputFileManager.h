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
}

class RootOutputFileHandle {

public:
    typedef std::map<std::string, bool> PathMap;
    typedef std::vector<TGeoManager*> GeoVector;
 
    RootOutputFileHandle(const std::string& filename, const std::map<std::string, int>& paths);
    ~RootOutputFileHandle();

    TFile* getFile();

    TTree* getNavTree();

    std::string getName();

    void setNavPath(const std::vector<std::string>& paths);

    void setNavEventName(const std::vector<std::string>& eventNames);

    void addTreeMetaData(JM::TreeMetaData* treemetadata);

    void addGeoManager(TGeoManager* geo);

    void addRef();

    int decRef();

    bool isLastPath(const std::string& path);

    void setNavAddr(void* navAddr);

    void close();

    void writing(const std::string& path);

private:
    TFile* m_file;
    TTree* m_navTree;
    JM::FileMetaData* m_fileMetaData;
    PathMap m_paths;
    GeoVector m_geos;
    int m_refCount;
    void* m_navAddr;
};

class RootOutputFileManager {

public:
    typedef std::map<std::string, RootOutputFileHandle*> FileMap; // map<file name,file handle>
    
    ~RootOutputFileManager();

    // Singleton
    static RootOutputFileManager* get();

    // Get file of given name, increase file's ref count
    RootOutputFileHandle* get_file(const std::string& filename, const std::string& path, int priority, const std::map<std::string, int>& otherPath);

    // Get file of given name
    RootOutputFileHandle* get_file(const std::string& filename);

    // Decrease file's ref count, really close when it hits zero.
    void close_file(const std::string& filename);

private:
    // Singleton
    RootOutputFileManager();

private:
    FileMap m_filemap;
    static RootOutputFileManager* m_fileMgr; // Pointer to self
};

#endif
