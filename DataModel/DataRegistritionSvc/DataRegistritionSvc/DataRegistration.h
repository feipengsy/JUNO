#ifndef DATA_REGISTRITION_H
#define DATA_REGISTRITION_H

#include <string>

class DataRegistration {

public:
    DataRegistration(const std::string& path, int priority, const std::string& headerName, const std::string& eventName)
        : m_path(path)
        , m_priority(priority)
        , m_headerName(headerName)
        , m_eventName(eventName)
    { }

    ~DataRegistration() {}

    int getPriority() { return m_priority; }

    std::string& getHeaderName() { return m_headerName; }

    std::string& getEventName() { return m_eventName; }

private:
    std::string m_path;
    int m_priority;
    std::string m_headerName, m_eventName;
};

#endif
