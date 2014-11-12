#include "RootIOSvc/RootOutputStream.h"
#include "RootIOSvc/RootFileWriter.h"
#include "RootIOUtil/RootOutputFileManager.h"
#include "EvtNavigator/EvtNavigator.h"
#include "Event/HeaderObject.h"
#include "SniperKernel/SniperLog.h"

#include <cstring>

RootOutputStream::RootOutputStream(const std::string& headername,
                                   const std::string& eventname,
                                   const std::string& treepath, 
                                   int priority,
                                   const PathMap& otherPath,
                                   DataRegistritionSvc* regSvc)
    : BaseIOStream("RootOutputStream") 
    , m_headerName(headername)
    , m_eventName(eventname)
    , m_path(treepath)
    , m_eventPriority(priority)
    , m_otherPaths(otherPath)
    , m_writer(new RootFileWriter(treepath, headername, eventname, regSvc))
{
}

RootOutputStream::~RootOutputStream()
{
    this->close();
    delete m_writer;
}

std::string RootOutputStream::path()
{
    return m_path;
}

int RootOutputStream::entries()
{
    return m_writer->entries();
}

bool RootOutputStream::write()
{
    // Write data and EvtNavigator(if nesserary)
    bool ok = m_writer->write();
    return ok;
}

bool RootOutputStream::close()
{
    if (!m_writer->getFile()) return true;
    LogDebug << "Closing file " << m_writer->getFile()->getName() 
             << std::endl;
    m_writer->close();
    return true;
}

bool RootOutputStream::attachObj(TObject* obj)
{
    RootOutputFileHandle* file =  m_writer->getFile();
    if (strcmp(obj->ClassName(), "TGeoManager") == 0) {
        file->addGeoManager(static_cast<TGeoManager*>(obj));
        return true;
    }
    // TODO Other object type... 
    return false;
}

bool RootOutputStream::setAddress(JM::EvtNavigator* nav)
{
    void* header = nav->getHeader(m_path);
    if (!header) return false;
    void* event = static_cast<JM::HeaderObject*>(header)->event();
    if (!event) return false;
    m_writer->setAddress(nav, header, event);
    return true;
}

bool RootOutputStream::newFile(const std::string& filename)
{
    // Only start a new file if filename differes
    if (m_writer->getFile() && filename == m_writer->getFile()->getName()) {
        return true;
    }

    this->close();

    bool ok = m_writer->newFile(RootOutputFileManager::get()->get_file(filename, m_path, m_eventPriority, m_otherPaths));

    if (ok) {
        LogDebug << "Start a new file: " << filename
                 << "for " << m_path
                 << std::endl;
    }

    return ok;
}
