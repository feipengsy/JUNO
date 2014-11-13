#ifndef DATA_REGISTRITION_SVC_H
#define DATA_REGISTRITION_SVC_H

#include "SniperKernel/SvcBase.h"
#include "DataRegistritionSvc/EDMRegistritionTable.h"
#include "DataRegistritionSvc/DataRegistrition.h"

#include <string>
#include <map>

class DataRegistritionSvc : public SvcBase {

public:

    typedef std::map<std::string, DataRegistrition*> Path2RegMap;

    DataRegistritionSvc(const std::string& name);
    ~DataRegistritionSvc();

    bool initialize();
    
    bool finalize();

    bool registerData(const std::string& eventName, std::string path = "Default");

    int getPriority(const std::string& path);

    std::string& getHeaderName(const std::string& path);

    std::string& getEventName(const std::string& path);

    DataRegistrition* getDataRegistrition(const std::string& path);

private:
    Path2RegMap m_registritions;
    EDMRegistritionTable* m_regTable;

    std::map<std::string, std::string> m_evt2path;
    
};


#endif
