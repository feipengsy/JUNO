#ifndef EDM_REGISTRITION_TABLE_H
#define EDM_REGISTRITION_TABLE_H

#include <vector>
#include <string>
#include <map>

class EDMRegistration {

    public:
        typedef std::vector<std::string> StringVector;

        EDMRegistration(const std::string& headerName, const StringVector& eventNames, int priority, const std::string& path) 
            : m_headerName(headerName)
            , m_eventNames(eventNames)
            , m_priority(priority)
            , m_path(path)
        {}

        ~EDMRegistration() {}

        int getPriority() { return m_priority; }
        const std::string& getHeaderName() const { return m_headerName; }
        const StringVector& getEventName() const { return m_eventNames; }
        const std::string& getPath() { return m_path; }

    private:
        std::string m_headerName;
        StringVector m_eventNames;
        int m_priority;
        std::string m_path;
};

class EDMManager {
    
    public:
        typedef std::vector<EDMRegistration*> RegVector;
        // Singleton
        static EDMManager* get();
        // Given event name, get default priority
        int getPriorityWithEvent(const std::string& name);
        // Given event name, get headerName
        std::string getHeaderNameWithEvent(const std::string& name);
        // Given event name, get default path
        std::string getPathWithEvent(const std::string& name);
        // Given header name, get default priority
        int getPriorityWithHeader(const std::string& name);
        // Given header name, get event name
        EDMRegistration::StringVector getEventNameWithHeader(const std::string& name);
        // Given header name, get default path
        std::string getPathWithHeader(const std::string& name);
        // Book a event type
        bool book(const std::string& headerName, const std::string& eventNames, int priority, const std::string& path);

    private:
        EDMManager();
        ~EDMManager();
        EDMRegistration* getRegWithEvent(const std::string& name);
        EDMRegistration* getRegWithHeader(const std::string& name);

    private:
        static EDMManager* s_manager;
        RegVector m_regs;
        std::string m_name;
        int m_refCount;
};

#endif
