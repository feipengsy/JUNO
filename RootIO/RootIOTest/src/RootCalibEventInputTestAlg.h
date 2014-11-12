#ifndef RootCalibEventInputTestAlg_h
#define RootCalibEventInputTestAlg_h

#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"

namespace JM {
    class CalibHeader;
    class CalibEvent;
    class CalibPMTChannel;
}

class RootCalibEventInputTestAlg : public AlgBase {

    public:
        RootCalibEventInputTestAlg(const std::string& name);
        virtual ~RootCalibEventInputTestAlg();

        virtual bool initialize();
        virtual bool execute();
        virtual bool finalize();
};

#endif
