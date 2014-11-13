#include "DataRegistritionSvc/EDMRegistritionTable.h"
#include "SniperKernel/SniperLog.h"

EDMRegistritionTable* EDMRegistritionTable::s_regTable = 0;

EDMRegistritionTable::EDMRegistritionTable() : m_name("EDMRegistritionTable"), m_refCount(0)
{
}

EDMRegistritionTable::~EDMRegistritionTable()
{
}

EDMRegistritionTable* EDMRegistritionTable::get()
{
    return (s_regTable != 0) ? s_regTable : (s_regTable = new EDMRegistritionTable);
}

int EDMRegistritionTable::getPriotiry(const std::string& name)
{
    Name2RegMap::iterator it = m_regs.find(name);
    if (it == m_regs.end()) {
        LogError << "Unkown data type: " << name
                 << std::endl;
        return -1;
    }
    return it->second->getPriority();
}

std::string EDMRegistritionTable::getHeaderName(const std::string& name)
{
    Name2RegMap::iterator it = m_regs.find(name);
    if (it == m_regs.end()) {
        LogError << "Unkown data type: " << name
                 << std::endl;
        return "Unkown";
    }
    return it->second->getHeaderName();
}

std::string EDMRegistritionTable::getPath(const std::string& name)
{
    Name2RegMap::iterator it = m_regs.find(name);
    if (it == m_regs.end()) {
        LogError << "Unkown data type: " << name
                 << std::endl;
        return "Unkown";
    }
    return it->second->getPath();
}

bool EDMRegistritionTable::book(std::string headerName, std::string eventName, int priority, std::string path)
{
    Name2RegMap::iterator it = m_regs.find(eventName);
    if (it != m_regs.end()) {
        LogFatal << "Duplicated data type: " << eventName
                 << std::endl;
        return false;
    }
    EDMRegistrition* reg = new EDMRegistrition(headerName, eventName, priority, path);
    m_regs.insert(std::make_pair(eventName, reg));
    return true;
}

