#include "RootIOUtil/RootOutputFileManager.h"

#include "TTree.h"
#include "TFile.h"
#include "TGeoManager.h"
#include "RootIOUtil/FileMetaData.h"
#include "UniqueIDTable.h"

#include <iostream>
#include <fstream>
#include <algorithm>

RootOutputFileHandle::RootOutputFileHandle(const std::string& filename, 
                                           const std::map<std::string, int>& paths)
    : m_file(TFile::Open(filename.c_str(),"recreate","RootOutputStream file"))
    , m_navTree(new TTree("navigator", "Tree for EvtNavigator"))
    , m_fileMetaData(new JM::FileMetaData())
    , m_IDTable(new JM::UniqueIDTable())
    , m_refCount(0)
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
    delete m_IDTable;
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

void addUniqueIDTable(const std::string& treename,
                      const std::vector<std::string>& guid,
                      const std::vector<std::vector<Int_t> >& uid,
                      const std::vector<std::vector<Short_t> >& bid)
{
    m_IDTable->AddTable(treename, guid, uid, bid);
}

int RootOutputFileHandle::decRef()
{
    return --m_refCount;
}

void RootOutputFileHandle::close()
{
    m_file->cd();

    // Write out file meta data
    m_fileMetaData->Write("FileMetaData");

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

bool RootOutputFileHandle::hasPath(const std::string& path)
{
    if (m_paths.find(path) != m_paths.end()) return true;
    return false; 
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

void RootOutputFileManager::new_file(const std::string& filename, 
                                     const std::map<std::string, int>& pathMap)
{
    FileMap::iterator it = m_filemap.find(filename);
    if (it == m_filemap.end()) {
        // Generate a new RootOutputFile with the paths and coresponding data priotity
        RootOutputFileHandle* filehandle = new RootOutputFileHandle(filename, pathMap);
        m_filemap[filename] = filehandle;
    }
    return;
}

RootOutputFileHandle* RootOutputFileManager::get_file_with_name(const std::string& filename) 
{
    FileMap::iterator it = m_filemap.find(filename);
    if (it == m_filemap.end()) return 0;
    return it->second;
}

RootOutputFileHandle* RootOutputFileManager::get_file_with_path(const std::string& path)
{
    FileMap::iterator it, end = m_filemap.end();
    for (it = m_filemap.begin(); it != end; ++it) {
        if (it->second->hasPath(path)) {
            it->second->addRef();
            return it->second;
        }
    }
    return 0;
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
