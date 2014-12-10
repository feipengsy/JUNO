#ifndef EVTNAVIGATOR_H
#define EVTNAVIGATOR_H

#include "TObject.h"
#include "TClass.h"
#include "TTimeStamp.h"
#include "RootIOUtil/SmartRef.h"
#include "Event/HeaderObject.h"
#include <vector>
#include <string>

namespace JM {

class EvtNavigator : public TObject {

public:

    EvtNavigator() { Class()->IgnoreTObjectStreamer(); }

    ~EvtNavigator();

    EvtNavigator(const EvtNavigator& nav);

    EvtNavigator& operator=(const EvtNavigator& nav);

    JM::HeaderObject* getHeader(const std::string& path);

    void addHeader(const std::string& path, JM::HeaderObject* header);

    void setHeaderEntry(const std::string& path, int entry);

    std::vector<std::string>& getPath();

    void setPath(const std::vector<std::string>& paths);

    void setWriteFlag(const std::string& path, bool write = false);

    bool writePath(const std::string& path);

    void resetWriteFlag();

    const TTimeStamp& TimeStamp() const;

    TTimeStamp& TimeStamp();

    void setTimeStamp(const TTimeStamp& value);
    
private:
    // paths and write flags won't be saved
    std::vector<std::string> m_paths; //!
    std::vector<bool> m_writeFlag; //!
    std::vector<JM::SmartRef*> m_refs;
    TTimeStamp m_TimeStamp; // Time stamp of event

    // Initialize self, using another EvtNavigator
    void init(const EvtNavigator& nav);

ClassDef(EvtNavigator,1)

};

} // namespace JM

#endif
