#ifndef ROOTIOUTIL_ROOTFILEWRITER_H
#define ROOTIOUTIL_ROOTFILEWRITER_H

#include "TObject.h"

#include <string>
#include <vector>

class RootOutputFileHandle;
class TTree;
class TDirectory;

namespace JM {
    class TreeMetaData;
    class EvtNavigator
}

class OutputTreeHandle {

    public:
        OutputTreeHandle();
        ~OutputTreeHandle();

        void setAddress(void* addr);
        unsigned int fill();

    private:
        TTree* m_tree;
        void*  m_addr;
        int    m_entries;
};

class RootFileWriter {

    public:
        typedef std::map<std::string, OutputTreeHandle*>  String2TreeHandle
        // Lazy-loading data types
        typedef std::vector<std::string>                  GUIDVector;
        typedef std::vector<std::vector<Short_t> >        BIDVector;
        typedef std::vector<std::vector<Int_t> >          UIDVector;
        
        RootFileWriter(const std::string& treepath, const std::string& headerName);
        ~RootFileWriter();
        // Get current file handle
        RootOutputFileHandle* getFile();
        // Write current state of addr to disk.
        bool write();
        // Write tree to file and close file.
        bool close();

        void revise();
        // Start a new output file
        void newFile(RootOutputFileHandle* file);
        // Return the number of entries in the output stream
        int entries();

        int fileEntries();

        void setAddress(JM::EvtNavigator* nav);
        // Set the name of event header
        void setHeaderName(const std::string& name);

    private:
        // Initialize the writer
        bool initialize();
        // Write header data to tree
        bool writeHeader();
        // Write event data to tree
        bool writeEvent();
        // Write Evtnavigators to tree
        bool writeNav();
        // Build data for auto-loading
        void fillBID(TObject* obj, int bid);
        // Check if this path is the last path of its output file
        void checkFilePath();
        /// Reset the addresses to 0
        void resetAddress();

    private:
        RootOutputFileHandle*   m_file;
        OutputTreeHandle*       m_headerTree;
        OutputTreeHandle*       m_navTree;
        String2TreeHandle       m_eventTrees;
        JM::TreeMetaData*       m_treeMetaData;
        TDirectory*             m_dir;
        std::string             m_path;
        bool                    m_withNav;
        int                     m_fileEntries;
   
        // For building TreeMetaData auto-loading data.
        GUIDVector              m_guid;
        BIDVector               m_bid;
        UIDVector               m_uid;
};


#endif
