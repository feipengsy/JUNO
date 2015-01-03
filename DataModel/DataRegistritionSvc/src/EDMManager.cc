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

int EDMManager::getPriorityWithEvent(const std::string& name)
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

int EDMManager::getPriorityWithHeader(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithHeader(name);
    return reg ? reg->getPriority() : -1;
}

EDMRegistration::StringVector EDMManager::getEventNameWithHeader(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithHeader(name);
    return reg ? reg->getEventName() : EDMRegistration::StringVector();
}

std::string EDMManager::getPathWithHeader(const std::string& name)
{
    EDMRegistration* reg = this->getRegWithHeader(name);
    return reg ? reg->getPath() : "unknown";
}

bool EDMManager::book(const std::string& headerName, const std::string& eventName, int priority, const std::string& path)
{
    RegVector::iterator it, end = m_regs.end();
    for (it = m_regs.begin(); it != end; ++it) {
        if ((*it)->getHeaderName() == headerName) {
            LogFatal << "Duplicated data type: " << headerName
                     << std::endl;
            return false;
        }
    }
    EDMRegistration::StringVector eventNames;
    std::string::size_type last = 0, apos = eventName.find('&');
    for (; apos != std::string::npos; apos = eventName.find('&',last)) {
        eventNames.push_back(eventName.substr(last, apos - last));
        last = apos + 1;
    }
    EDMRegistration* reg = new EDMRegistration(headerName, eventNames, priority, path);
    m_regs.push_back(reg);
    return true;
}

EDMRegistration* EDMManager::getRegWithEvent(const std::string& name)
{
    RegVector::iterator it, end = m_regs.end();
    for (it = m_regs.begin(); it != end; ++it) {
        const EDMRegistration::StringVector& eventNames = (*it)->getEventName();
        if (std::find(eventNames.begin(), eventNames.end(), name) != eventNames.end()) {
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

