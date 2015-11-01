/*  class RootInputSvc
 *  
 *  RootInputSvc is designed to configure input files and intialize
 *  RootInputStream.
 *
 */
#ifndef ROOT_INPUT_SVC_H
#define ROOT_INPUT_SVC_H 0

#include "SniperKernel/SvcBase.h"
#include "RootIOSvc/IInputSvc.h"

class TObject;

class RootInputSvc: public SvcBase, public IInputSvc {

    public:
        RootInputSvc(const std::string& name);
        ~RootInputSvc();

        // Service interface
        bool initialize();
        bool finalize();
        // Get the nav input stream
        IInputStream* getInputStream(const std::string& path);
        // Get the additional TObject of one file
        bool getObj(TObject*& obj, const std::string& objName, const std::string& path="");

    private:
        bool initPlainStream();
        bool initNavStream();

    private:
        InputFileList   m_navInputFile;    // List of input files holding EvtNavigators.(If set, NavInputStream will be created)
        InputFileMap    m_inputFileMap;    // Map of path to input files
        InputStreamMap  m_inputStream;     // Map of path to input stream
};

#endif
