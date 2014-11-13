#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "DataRegistritionSvc/EDMRegistritionTable.h"
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

    m_regTable = EDMRegistritionTable::get();

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
    int defaultPriotiry = m_regTable->getPriotiry(eventName);
    if (-1 == defaultPriotiry) {
        LogError << "Failed to get default priority for " << eventName
                 << std::endl;
        return false;
    }
    std::string headerName = m_regTable->getHeaderName(eventName);
    if (path == "Default") path = m_regTable->getPath(eventName);
    if (m_registritions.find(path) != m_registritions.end()) {
        LogError << "Duplicated data path: " << path
                 << std::endl;
        return false;
    }
    int repetition = 0;
    Path2RegMap::iterator it, end = m_registritions.end();
    for (it = m_registritions.begin(); it != end; ++it) {
        if (eventName == it->second->getEventName()) ++repetition;
    }
    defaultPriotiry += repetition;

    DataRegistrition* registrition = new DataRegistrition(path, defaultPriotiry, headerName, eventName);
    m_registritions.insert(std::make_pair(path, registrition));
    LogDebug << "Registered data path :" << path
             << std::endl;
    return true;
}

int DataRegistritionSvc::getPriority(const std::string& path)
{
    DataRegistrition* reg = getDataRegistrition(path);
    if (!reg) return 0;
    return reg->getPriority();
}

std::string& DataRegistritionSvc::getHeaderName(const std::string& path)
{
    DataRegistrition* reg = getDataRegistrition(path);
    return reg->getHeaderName();
}

std::string& DataRegistritionSvc::getEventName(const std::string& path)
{
    DataRegistrition* reg = getDataRegistrition(path);
    return reg->getEventName();
}

DataRegistrition* DataRegistritionSvc::getDataRegistrition(const std::string& path)
{
    std::map<std::string, DataRegistrition*>::iterator it = m_registritions.find(path);
    if (it == m_registritions.end()) return 0;
    return it->second;
}
