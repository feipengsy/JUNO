#include "RootMultiTaskTestAlg.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/Incident.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "RootIOSvc/RootOutputSvc.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include "Event/PhyHeader.h"
#include "Event/PhyEvent.h"

#include <sstream>

DECLARE_ALGORITHM(RootMultiTaskTestAlg);

RootMultiTaskTestAlg::RootMultiTaskTestAlg(const std::string& name)
    : AlgBase(name)
{
    m_count = 0;
}

RootMultiTaskTestAlg::~RootMultiTaskTestAlg()
{
}

bool RootMultiTaskTestAlg::initialize()
{
    // Get DataRegistritionSvc and register the data type and path to be generated.
    SniperPtr<DataRegistritionSvc> drsSvc(getScope(), "DataRegistritionSvc");
    if ( ! drsSvc.valid() ) {
        LogError << "Failed to get DataRegistritionSvc instance!" << std::endl;
        return false;
    }
    // Will Generate JM::PhyEvent
    drsSvc->registerData("JM::PhyEvent", "/Event/PhyEvent");

    return true;

}

bool RootMultiTaskTestAlg::execute()
{
    ++m_count;

    SniperPtr<IDataMemMgr> mMgr(getScope(), "BufferMemMgr");

    // Triger the sub-tasks and get event from "their" buffer
    std::stringstream path;
    // Triger in order
    int i = m_count % 3;
    path << "subTask" << i;
    Incident::fire(path.str());

    // Get "their" event, and accordingly, generate event for top_task
    // Get the data buffer of sub_tasks
    path << ":/Event";
    SniperDataPtr<JM::NavBuffer> subNavBuf(getScope(), path.str());
    if ( subNavBuf.invalid() ) {
        LogError << "cannot get the NavBuffer subTask" << i << ":/Event" << std::endl;
        return false;
    }
    JM::EvtNavigator* nav = new JM::EvtNavigator();
    JM::PhyHeader* ph = new JM::PhyHeader();
    JM::PhyEvent* pe = new JM::PhyEvent();
    JM::PhyEvent* event = (JM::PhyEvent*)subNavBuf->curEvt()->getHeader("/Event/PhyEvent")->event();
    pe->setEnergy(event->energy());
    pe->setRawEvis(event->rawEvis());
    pe->setEnrec(event->enrec());
    pe->setEprec(event->eprec());
    ph->setEvent(pe);
    nav->addHeader("/Event/PhyEvent", ph);
    mMgr->adopt(nav, "/Event");
    path.str("");

    LogInfo << "executing: " << m_count
            << std::endl;

    return true;
}

bool RootMultiTaskTestAlg::finalize()
{
    LogInfo << " finalized successfully" << std::endl;

    return true;
}
