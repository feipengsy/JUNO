#ifndef EDM_REGISTRITION_TABLE_H
#define EDM_REGISTRITION_TABLE_H

#include <string>
#include <map>

class EDMRegistrition {

public:

    EDMRegistrition(const std::string& headerName, const std::string& eventName, int priority, std::string& path) 
        : m_headerName(headerName)
        , m_eventName(eventName)
        , m_priority(priority)
        , m_path(path)
    {}

    ~EDMRegistrition() {}

    int getPriority() { return m_priority; }

    std::string getHeaderName() { return m_headerName; }

    std::string getPath() { return m_path; }

private:

    std::string m_headerName;
    std::string m_eventName;
    int m_priority;
    std::string m_path;

};

class EDMRegistritionTable {

public:

    typedef std::map<std::string, EDMRegistrition*> Name2RegMap;

    // Singleton
    static EDMRegistritionTable* get();

    // Given event name, get default priority
    int getPriotiry(const std::string& name);

    // Given event name, get headerName
    std::string getHeaderName(const std::string& name);

    // Given event name, get default path
    std::string getPath(const std::string& name);

    // Book a event type
    bool book(std::string headerName, std::string eventName, int priority, std::string path);

private:

    EDMRegistritionTable();
    ~EDMRegistritionTable();

    static EDMRegistritionTable* s_regTable;
    Name2RegMap m_regs;
    std::string m_name;
    int m_refCount;

};

#endif
