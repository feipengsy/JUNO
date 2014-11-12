#include "RootCalibEventInputTestAlg.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SniperLog.h"
#include "EvtNavigator/NavBuffer.h"
#include "BufferMemMgr/IDataMemMgr.h"

#include "Event/CalibHeader.h"

#include <list>

DECLARE_ALGORITHM(RootCalibEventInputTestAlg);

RootCalibEventInputTestAlg::RootCalibEventInputTestAlg(
        const std::string& name)
    : AlgBase(name)
{

}

RootCalibEventInputTestAlg::~RootCalibEventInputTestAlg()
{

}

bool
RootCalibEventInputTestAlg::initialize()
{
    return true;
}

bool
RootCalibEventInputTestAlg::execute()
{

    // = Load data from buffer =
    SniperDataPtr<JM::NavBuffer>  navBuf(getScope(), "/Event");
    if (navBuf.invalid()) {
        return false;
    }
    LogDebug << "navBuf: " << navBuf.data() << std::endl;
    JM::EvtNavigator* evt_nav = navBuf->curEvt();
    LogDebug << "evt_nav: " << evt_nav << std::endl;
    if (not evt_nav) {
        return false;
    }
    JM::CalibHeader* chcol =(JM::CalibHeader*) evt_nav->getHeader("/Event/CalibEvent"); 
    const std::list<JM::CalibPMTChannel*>& chhlist = chcol->event()->calibPMTCol();

    // = Display the data in the buffer =
    LogDebug << "collection size: " << chhlist.size() << std::endl;

    return true;
}

bool
RootCalibEventInputTestAlg::finalize()
{
    return true;
}
