#ifndef ROOTIOUTIL_ROOTFILEWRITER_H
#define ROOTIOUTIL_ROOTFILEWRITER_H

#include "TObject.h"

#include <string>
#include <vector>

class RootOutputFileHandle;
class DataRegistritionSvc;
class TTree;
class TDirectory;
class TObject;

namespace JM {
    class TreeMetaData;
}

class RootFileWriter {

public:
    typedef std::vector<std::string> GUIDVector;
    typedef std::vector<std::vector<Short_t> > BIDVector;
    typedef std::vector<std::vector<Int_t> > UIDVector;

    RootFileWriter(const std::string& treepath, const std::string& headername, const std::string& eventname, DataRegistritionSvc* regSvc);
    ~RootFileWriter();
    /// Get current file handle
    RootOutputFileHandle* getFile();
    /// Write current state of addr to disk.
    bool write();
    /// Write tree to file and close file.
    bool close();

    void revise();
    /// Start a new output file
    bool newFile(RootOutputFileHandle* file);
    /// Return the number of entries in the output stream
    int entries();

    int fileEntries();

    void setAddress(void* nav, void* header, void* event);
    /// Set the name of event header
    void setHeaderName(const std::string& name);
    /// Set the name of event
    void setEventName(const std::string& name);

private:
    /// Initialize the writer
    bool initialize();
    /// Write event data to tree
    bool writeData();
    /// Write Evtnavigators to tree
    bool writeNav();
    /// Build data for auto-loading
    void fillBID(TObject* obj, int bid);
    // Check if this path is the last path of its output file
    void checkFilePath();

private:
    RootOutputFileHandle* m_file;
    TTree* m_tree;
    TTree* m_navTree;
    JM::TreeMetaData* m_treeMetaData;
    TDirectory* m_dir;
    std::string m_headerName, m_eventName, m_path;
    bool m_withNav;
    int m_entries, m_fileEntries;
    void* m_headerAddr;
    void* m_eventAddr;
    void* m_navAddr;
    DataRegistritionSvc* m_regSvc;
   
    // For building TreeMetaData auto-loading data.
    GUIDVector m_guid;
    BIDVector m_bid;
    UIDVector m_uid;
};


#endif
