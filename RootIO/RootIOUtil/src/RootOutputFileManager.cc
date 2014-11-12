#include "RootIOUtil/RootOutputFileManager.h"

#include "TTree.h"
#include "TFile.h"
#include "TGeoManager.h"
#include "RootIOUtil/FileMetaData.h"

#include <iostream>
#include <fstream>
#include <algorithm>

RootOutputFileHandle::RootOutputFileHandle(const std::string& filename, 
                                           const std::map<std::string, int>& paths)
    : m_file(TFile::Open(filename.c_str(),"recreate","RootOutputStream file"))
    , m_navTree(new TTree("navigator", "Tree for EvtNavigator"))
    , m_fileMetaData(new JM::FileMetaData())
    , m_refCount(1)
    , m_navAddr(0)
{
    std::map<std::string, int>::const_iterator it, end = paths.end();
    int pri = 0;
    for (it = paths.begin(); it != end; ++it) {
        m_paths.insert(std::make_pair(it->first, false));
        if (it->second > pri) pri = it->second;
    }
    m_fileMetaData->SetNavPriority(pri);
    m_navTree->Branch("EvtNavigator", "JM::EvtNavigator", &m_navAddr);
}

RootOutputFileHandle::~RootOutputFileHandle()
{
    delete m_file;
    delete m_fileMetaData;
}

std::string RootOutputFileHandle::getName()
{
    return m_file->GetName();
}

TFile* RootOutputFileHandle::getFile()
{
    return m_file;
}

void RootOutputFileHandle::addRef()
{
    ++m_refCount;
}

void RootOutputFileHandle::addGeoManager(TGeoManager* geo)
{
    if (std::find(m_geos.begin(), m_geos.end(), geo) == m_geos.end()) {
        m_geos.push_back(geo);
    }
}

int RootOutputFileHandle::decRef()
{
    return --m_refCount;
}

void RootOutputFileHandle::close()
{
    m_file->cd();

    // Write out file meta data
    m_fileMetaData->Write();

    // Write out geometry infomation

    if (m_geos.size()) {
        // Shield noizy log info for the moment
        std::streambuf *backup;
        std::ofstream fout;
        fout.open("/dev/null");
        backup = std::cout.rdbuf();
        std::cout.rdbuf(fout.rdbuf());
        GeoVector::iterator it, end = m_geos.end();
        m_file->mkdir("Geometry");
        m_file->cd("Geometry");
        for (it = m_geos.begin(); it != end; ++it) {
            if (strlen((*it)->GetName()) == 0) (*it)->SetName("JunoGeo");
            (*it)->Write();
        }
        std::cout.rdbuf(backup);
    }    
    m_file->Close(); 
}

void RootOutputFileHandle::writing(const std::string& path)
{
    PathMap::iterator it = m_paths.find(path);
    // Path should always be found
    it->second = true;
}

void RootOutputFileHandle::setNavPath(const std::vector<std::string>& paths)
{
    m_fileMetaData->SetNavPath(paths);
}

void RootOutputFileHandle::setNavEventName(const std::vector<std::string>& eventNames)
{
    m_fileMetaData->SetNavEventName(eventNames);
}
void RootOutputFileHandle::addTreeMetaData(JM::TreeMetaData* treemetadata)
{
    m_fileMetaData->AddTreeMetaData(treemetadata);
}

TTree* RootOutputFileHandle::getNavTree()
{
    return m_navTree;
}

bool RootOutputFileHandle::isLastPath(const std::string& path)
{
    PathMap::iterator it, end = m_paths.end();
    for (it = m_paths.begin(); it != end; ++it) {
        if (it->first == path) continue;
        if (it->second == false) return false;
    }
    return true;
}

void RootOutputFileHandle::setNavAddr(void* navAddr)
{
    m_navAddr = navAddr;
}

/*   RootOutputFileManager   */

RootOutputFileManager* RootOutputFileManager::m_fileMgr = 0;

RootOutputFileManager::RootOutputFileManager()
{
}

RootOutputFileManager::~RootOutputFileManager()
{
}

RootOutputFileManager* RootOutputFileManager::get()
{
    if (!m_fileMgr) m_fileMgr = new RootOutputFileManager();
    return m_fileMgr;
}

RootOutputFileHandle* RootOutputFileManager::get_file(const std::string& filename, 
                                                     const std::string& path, 
                                                     int priority, 
                                                     const std::map<std::string, int>& otherPath)
{
    FileMap::iterator it = m_filemap.find(filename);
    if (it == m_filemap.end()) {  // miss

        // Generate a new RootOutputFile with the paths and coresponding data priotity
        std::map<std::string, int> paths = otherPath;
        paths.insert(std::make_pair(path,priority));
        RootOutputFileHandle* filehandle = new RootOutputFileHandle(filename, paths);
        m_filemap[filename] = filehandle;
        filehandle->writing(path);
        return filehandle;
    }
    // hit
    it->second->addRef();
    it->second->writing(path);
    return it->second;
}

RootOutputFileHandle* RootOutputFileManager::get_file(const std::string& filename) 
{
    FileMap::iterator it = m_filemap.find(filename);
    if (it == m_filemap.end()) return 0;
    return it->second;
}

void RootOutputFileManager::close_file(const std::string& filename)
{
    FileMap::iterator it = m_filemap.find(filename);
    if (it == m_filemap.end()) {
        // Can not find the file, may be already closed.
        return;
    }

    int refCount = it->second->decRef();
    if (refCount) return;  // still more references

    it->second->close();
    // Delete the file when it's closed
    delete it->second;
    m_filemap.erase(it); 
}
