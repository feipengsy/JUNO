#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "DataRegistritionSvc/EDMManager.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"

DECLARE_SERVICE(DataRegistritionSvc);

DataRegistritionSvc::DataRegistritionSvc(const std::string& name) : SvcBase(name)
{
    declProp("EventToPath", m_evt2path);
}

DataRegistritionSvc::~DataRegistritionSvc()
{
}

bool DataRegistritionSvc::initialize()
{
    LogDebug << "Initializing DataRegistritionSvc..."
            << std::endl;

    m_EDMMgr = EDMManager::get();

    for (std::map<std::string, std::string>::iterator it = m_evt2path.begin();
            it != m_evt2path.end(); ++it) {
        registerData(it->first, it->second);
    }

    LogDebug << "DataRegistritionSvc initialized."
            << std::endl;

    return true;
}

bool DataRegistritionSvc::finalize()
{
    LogDebug << "Finalizing DataRegistritionSvc..."
            << std::endl;

    LogDebug << "DataRegistritionSvc finalized."
            << std::endl;
    return true;
}

bool DataRegistritionSvc::registerData(const std::string& eventName, std::string path)
{
    int defaultPriotiry = m_EDMMgr->getPriotiryWithEvent(eventName);
    if (-1 == defaultPriotiry) {
        LogError << "Failed to get default priority for " << eventName
                 << std::endl;
        return false;
    }
    std::string headerName = m_EDMMgr->getHeaderNameWithEvent(eventName);
    if (path == "Default") path = m_EDMMgr->getPathWithEvent(eventName);
    if (m_registrations.find(path) != m_registrations.end()) {
        LogError << "Duplicated data path: " << path
                 << std::endl;
        return false;
    }
    int repetition = 0;
    Path2RegMap::iterator it, end = m_registrations.end();
    for (it = m_registrations.begin(); it != end; ++it) {
        if (eventName == it->second->getEventName()) ++repetition;
    }
    defaultPriotiry += repetition;

    DataRegistration* registration = new DataRegistration(path, defaultPriotiry, headerName, eventName);
    m_registrations.insert(std::make_pair(path, registration));
    LogDebug << "Registered data path :" << path
             << std::endl;
    return true;
}

int DataRegistritionSvc::getPriority(const std::string& path)
{
    DataRegistration* reg = getDataRegistration(path);
    if (!reg) return 0;
    return reg->getPriority();
}

std::string& DataRegistritionSvc::getHeaderName(const std::string& path)
{
    DataRegistration* reg = getDataRegistration(path);
    return reg->getHeaderName();
}

std::string& DataRegistritionSvc::getEventName(const std::string& path)
{
    DataRegistration* reg = getDataRegistration(path);
    return reg->getEventName();
}

DataRegistrition* DataRegistritionSvc::getDataRegistration(const std::string& path)
{
    std::map<std::string, DataRegistrition*>::iterator it = m_registrations.find(path);
    if (it == m_registrations.end()) return 0;
    return it->second;
}
