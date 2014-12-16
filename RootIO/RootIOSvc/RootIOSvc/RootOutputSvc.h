#ifndef ROOTIOSVC_ROOTOUTPUTSVC_H
#define ROOTIOSVC_ROOTOUTPUTSVC_H

#include "RootIOSvc/BaseIOSvc.h"
#include "EvtNavigator/EvtNavigator.h"

#include <map>
#include <string>
#include <vector>

class RootOutputStream;
class DataRegistritionSvc;

class RootOutputSvc: public BaseIOSvc {

public:

    typedef std::map<std::string,std::string> OutputFileMap; // { path: filename }
    typedef std::multimap<std::string, TObject*> OutputObjMap; // {path: object}
    typedef std::vector<RootOutputStream*> OutputStreamVector;

    RootOutputSvc(const std::string& name);

    ~RootOutputSvc();

    // Service interface
    bool initialize();
    bool finalize();

    bool initializeOutputStream();

    bool write(JM::EvtNavigator* nav);

    // Attach additional TObject to be written to the stream.
    bool attachObj(const std::string& path, TObject* obj);
 
private:
    OutputFileMap m_outputFileMap;
    OutputObjMap m_exOutputObjs;
    OutputStreamVector m_outputStreams;
    DataRegistritionSvc* m_regSvc;
    bool m_streamInitialized;

    bool initializeOutputStream(const JM::EvtNavigator* nav);
    bool doAttachObj(const std::string& path, TObject* obj);
};


#endif
