#ifndef ROOTIOSVC_ROOTOUTPUTSTREAM_H
#define ROOTIOSVC_ROOTOUTPUTSTREAM_H

#include "RootIOSvc/BaseIOStream.h"

#include <map>
#include <string>

namespace JM {
    class EvtNavigator;
}

class RootFileWriter;
class DataRegistritionSvc;
class TObject;

class RootOutputStream : public BaseIOStream {

public:

    // Other paths to be written to the same file in this stream(if any)
    typedef std::map<std::string, int> PathMap;
    
    RootOutputStream(const std::string& headername, 
                     const std::string& eventname, 
                     const std::string& treepath,
                     DataRegistritionSvc* regSvc);
    virtual ~RootOutputStream();

    /// RootIOStream interface
    std::string path();

    /// Write current state of addr to disk.
    bool write();

    /// Write tree to file and close file.
    bool close();

    void revise();

    bool setAddress(JM::EvtNavigator* nav);

    /// Start a new output file
    bool newFile(const std::string& filename);
    /// Return the number of entries in the output stream
    int entries();

private:
    std::string m_headerName, m_eventName, m_path;
    RootFileWriter* m_writer;
};

#endif
