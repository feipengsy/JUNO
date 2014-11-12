#include "RootSimEventOutputTestAlg.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include "Event/SimHeader.h"
#include "Event/SimEvent.h"

DECLARE_ALGORITHM(RootSimEventOutputTestAlg);

RootSimEventOutputTestAlg::RootSimEventOutputTestAlg(const std::string& name)
    : AlgBase(name) 
    , m_buf(0)
{
    m_iEvt = 0;

    declProp("TotalPE", m_totalpe=100000);
}

RootSimEventOutputTestAlg::~RootSimEventOutputTestAlg()
{
}

bool RootSimEventOutputTestAlg::initialize()
{
    SniperDataPtr<JM::NavBuffer>  navBuf(getScope(), "/Event");
    if ( navBuf.invalid() ) {
        LogError << "cannot get the NavBuffer @ /Event" << std::endl;
        return false;
    }
    m_buf = navBuf.data();

    SniperPtr<DataRegistritionSvc> drsSvc(getScope(), "DataRegistritionSvc");
    if ( ! drsSvc.valid() ) {
        LogError << "Failed to get DataRegistritionSvc instance!" << std::endl;
        return false;
    }
    drsSvc->registerData("JM::SimEvent", "/Event/SimEvent");

    return true;

}

bool RootSimEventOutputTestAlg::execute()
{
    ++m_iEvt;

    //create EvtNavigator, set TimeStamp
    JM::EvtNavigator* nav = new JM::EvtNavigator();
    static TTimeStamp time(2014, 7, 29, 10, 10, 2, 111);
    time.Add(TTimeStamp(0, abs(m_r.Gaus(200000, 200000/5))));
    nav->setTimeStamp(time);

    //put EvtNavigator into data buffer
    SniperPtr<IDataMemMgr> mMgr(getScope(), "BufferMemMgr");
    mMgr->adopt(nav, "/Event");

    //set headers and events ...
    // = event =
    JM::SimEvent* event = new JM::SimEvent();
    // == generate dummy data ==
    generate_hits(event);
    generate_trks(event);

    // = header =
    JM::SimHeader* header = new JM::SimHeader();
    header->setEvent(event);
    nav->addHeader("/Event/SimEvent", header);

    LogDebug << "executing: " << m_iEvt
             << "  buffer_size: " << m_buf->size()
             << std::endl;

    return true;
}

bool RootSimEventOutputTestAlg::finalize()
{
    LogInfo << " finalized successfully" << std::endl;

    return true;
}

void RootSimEventOutputTestAlg::generate_hits(JM::SimEvent* event)
{
    // assume there are 1000000 hits

    for (int i = 0; i < m_totalpe; ++i) {
        JM::SimPMTHit* jm_hit = event->addCDHit();
        jm_hit -> setPMTID(m_r.Integer(20000));
        jm_hit -> setNPE(m_r.Integer(2000));
        jm_hit -> setHitTime(m_r.Integer(2000));

    }
}

void RootSimEventOutputTestAlg::generate_trks(JM::SimEvent* event)
{

}
