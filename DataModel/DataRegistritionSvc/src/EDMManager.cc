#include "DataRegistritionSvc/EDMManager.h"
#include "SniperKernel/SniperLog.h"

EDMManager* EDMManager::s_manager = 0;

EDMManager::EDMManager() : m_name("EDMManager"), m_refCount(0)
{
}

EDMManager::~EDMManager()
{
}

EDMManager* EDMManager::get()
{
    return (s_manager != 0) ? s_manager : (s_manager = new EDMManager);
}

int EDMManager::getPriotiryWithEvent(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithEvent(name);
    return reg ? reg->getPriority() : -1;
}

std::string EDMManager::getHeaderNameWithEvent(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithEvent(name);
    return reg ? reg->getHeaderName() : "unknown";
}

std::string EDMManager::getPathWithEvent(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithEvent(name);
    return reg ? reg->getPath() : "unknown";
}

int EDMManager::getPriotiryWithHeader(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithHeader(name);
    return reg ? reg->getPriority() : -1;
}

std::string EDMManager::getEventNameWithHeader(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithHeader(name);
    return reg ? reg->getEventName() : "unknown";
}

std::string EDMManager::getPathWithHeader(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithHeader(name);
    return reg ? reg->getPath() : "unknown";
}

bool EDMManager::book(std::string headerName, std::string eventName, int priority, std::string path)
{
    RegVector::iterator it, end = m_regs.end();
    for (it = m_regs.begin(); it != end; ++it) {
        if ((*it)->getEventName() == eventName) {
            LogFatal << "Duplicated data type: " << eventName
                     << std::endl;
            return false;
        }
    }
    EDMRegistration* reg = new EDMRegistration(headerName, eventName, priority, path);
    m_regs.push_back(reg);
    return true;
}

EDMRegistration* EDMManager::getRegWithEvent(const std::string& name)
{
    RegVector::iterator it, end = m_regs.end();
    for (it = m_regs.begin(); it != end; ++it) {
        if ((*it)->getEventName() == name) {
            return *it;
        }
    }
    LogError << "Unknown data event type: " << name
             << std::endl;
    return 0;
}

EDMRegistration* EDMManager::getRegWithHeader(const std::string& name)
{
    RegVector::iterator it, end = m_regs.end();
    for (it = m_regs.begin(); it != end; ++it) {
        if ((*it)->getHeaderName() == name) {
            return *it;
        }
    }
    LogError << "Unknown data header type: " << name
             << std::endl;
    return 0;
}

