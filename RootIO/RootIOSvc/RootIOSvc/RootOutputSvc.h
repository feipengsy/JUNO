#ifndef ROOTIOSVC_ROOTOUTPUTSVC_H
#define ROOTIOSVC_ROOTOUTPUTSVC_H

#include "RootIOSvc/BaseIOSvc.h"
#include "EvtNavigator/EvtNavigator.h"

#include <map>
#include <string>
#include <vector>

class RootOutputStream;

class RootOutputSvc: public BaseIOSvc {

public:

    typedef std::map<std::string,std::string>     String2String;
    typedef std::multimap<std::string, TObject*>  OutputObjMap; // {path: object}
    typedef std::vector<RootOutputStream*>        OutputStreamVector;
    typedef std::vector<std::string>              StringVector;

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
    bool initializeOutputStream(JM::EvtNavigator* nav);
    bool reviseOutputStream(const std::string& path, const std::string& headerName);
    bool doAttachObj(const std::string& path, TObject* obj);

private:
    String2String          m_outputFileMap, m_path2typeMap;
    OutputObjMap           m_exOutputObjs;
    OutputStreamVector     m_outputStreams;
    StringVector           m_notYetInitPaths;
    bool                   m_streamInitialized;
};


#endif
