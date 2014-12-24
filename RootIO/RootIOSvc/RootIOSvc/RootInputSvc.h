/*  class RootInputSvc
 *  
 *  RootInputSvc is designed to configure input files and intialize
 *  RootInputStream and InputElementKeeper.
 *
 */
#ifndef ROOTINPUTSVC_H
#define ROOTINPUTSVC_H

#include <vector>
#include <string>
#include "RootIOSvc/BaseIOSvc.h"

class RootInputStream;
class InputElementKeeper;
class TObject;

class RootInputSvc: public BaseIOSvc {

public:
    RootInputSvc(const std::string& name);

    ~RootInputSvc();

    // Service interface
    bool initialize();
    bool finalize();

    // Get the nav input stream
    RootInputStream* getInputStream();

    // Get the additional TObject of the stream
    bool getObj(TObject*& obj, const std::string& name, const std::string& path="none");

private:

    std::vector<std::string> m_inputFile;
    RootInputStream* m_inputStream;
    InputElementKeeper* m_keeper;
};

#endif
