#ifndef DATA_REGISTRITION_SVC_H
#define DATA_REGISTRITION_SVC_H

#include "SniperKernel/SvcBase.h"
#include "DataRegistritionSvc/EDMManager.h"
#include "DataRegistritionSvc/DataRegistration.h"

#include <string>
#include <map>

class DataRegistritionSvc : public SvcBase {

public:
    typedef std::map<std::string, DataRegistration*> Path2RegMap;

    DataRegistritionSvc(const std::string& name);
    ~DataRegistritionSvc();

    bool initialize();
    
    bool finalize();

    bool registerData(const std::string& eventName, std::string path = "Default");

    int getPriority(const std::string& path);

    std::string& getHeaderName(const std::string& path);

    std::string& getEventName(const std::string& path);

private:
    DataRegistration* getDataRegistration(const std::string& path);

private:
    Path2RegMap m_registrations;
    EDMManager* m_EDMMgr;
    std::map<std::string, std::string> m_evt2path;
    
};


#endif
