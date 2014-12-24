#ifndef EDM_REGISTRITION_TABLE_H
#define EDM_REGISTRITION_TABLE_H

#include <vector>
#include <map>

class EDMRegistration {

public:

    EDMRegistration(const std::string& headerName, const std::string& eventName, int priority, std::string& path) 
        : m_headerName(headerName)
        , m_eventName(eventName)
        , m_priority(priority)
        , m_path(path)
    {}

    ~EDMRegistration() {}

    int getPriority() { return m_priority; }
    std::string& getHeaderName() { return m_headerName; }
    std::string& getEventName() { return m_eventName; }
    std::string& getPath() { return m_path; }

private:

    std::string m_headerName;
    std::string m_eventName;
    int m_priority;
    std::string m_path;

};

class EDMManager {
    
    public:
        typedef std::vector<EDMRegistration*> RegVector;
        // Singleton
        static EDMManager* get();
        // Given event name, get default priority
        int getPriotiryWithEvent(const std::string& name);
        // Given event name, get headerName
        std::string getHeaderNameWithEvent(const std::string& name);
        // Given event name, get default path
        std::string getPathWithEvent(const std::string& name);
        // Given header name, get default priority
        int getPriotiryWithHeader(const std::string& name);
        // Given header name, get event name
        std::string getHeaderNameWithHeader(const std::string& name);
        // Given header name, get default path
        std::string getPathWithHeader(const std::string& name);
        // Book a event type
        bool book(std::string headerName, std::string eventName, int priority, std::string path);

    private:
        EDMManager();
        ~EDManager();
        EDMRegistration* getRegWithEvent(const std::string& name);
        EDMRegistration* getRegWithHeader(const std::string& name);

    private:
        static EDMManager* s_manager;
        RegVector m_regs;
        std::string m_name;
        int m_refCount;
};

#endif
