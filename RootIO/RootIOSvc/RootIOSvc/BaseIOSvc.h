/*
 * class BaseIOSvc
 * 
 * Base class of RootInputSvc and RootOutputSvc
 *
 */
#ifndef BASEIOSVC_H
#define BASEIOSVC_H

#include <string>
#include "SniperKernel/SvcBase.h"

class BaseIOSvc : public SvcBase {

public:
    BaseIOSvc(const std::string name) : SvcBase(name) {}
    virtual ~BaseIOSvc() {}
    virtual bool initlize() { return true; }
    virtual bool finalize() { return true; }

};

#endif
