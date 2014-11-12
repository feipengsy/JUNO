#include "RootOutputTestAlg.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "RootIOSvc/RootOutputSvc.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include "Event/PhyHeader.h"
#include "Event/PhyEvent.h"

#include "TGeoManager.h"

#include <fstream>

DECLARE_ALGORITHM(RootOutputTestAlg);

RootOutputTestAlg::RootOutputTestAlg(const std::string& name)
    : AlgBase(name) 
    , m_buf(0)
{
    m_iEvt = 0;
}

RootOutputTestAlg::~RootOutputTestAlg()
{
}

bool RootOutputTestAlg::initialize()
{
    // Get the data buffer
    SniperDataPtr<JM::NavBuffer>  navBuf(getScope(), "/Event");
    if ( navBuf.invalid() ) {
        LogError << "cannot get the NavBuffer @ /Event" << std::endl;
        return false;
    }
    m_buf = navBuf.data();


    // Get DataRegistritionSvc and register the data type and path to be generated.
    SniperPtr<DataRegistritionSvc> drsSvc(getScope(), "DataRegistritionSvc");
    if ( ! drsSvc.valid() ) {
        LogError << "Failed to get DataRegistritionSvc instance!" << std::endl;
        return false;
    }
    // Will Generate JM::PhyEvent
    drsSvc->registerData("JM::PhyEvent", "/Event/PhyEvent");


    // Attach the geometry to output file
    // Create TGeoManager
    // Get the RootOutputSvc
    SniperPtr<RootOutputSvc> ros(getScope(), "OutputSvc");
    if ( ! ros.valid() ) {
        LogError << "Failed to get RootOutputSvc instance!" << std::endl;
        return false;
    }
    // Attach TGeoManager to output stream
    std::streambuf *backup; 
    std::ofstream fout; 
    fout.open("/dev/null"); 
    backup = std::cout.rdbuf(); 
    std::cout.rdbuf(fout.rdbuf()); 
    TGeoManager* geom = TGeoManager::Import("geometry_test.gdml"); 
    std::cout.rdbuf(backup);
    geom->SetName("JunoGeom"); 

    ros->attachObj("/Event/PhyEvent", geom);

    return true;

}

bool RootOutputTestAlg::execute()
{
    ++m_iEvt;

    // Create Dummy EvtNavigator, set TimeStamp
    JM::EvtNavigator* nav = new JM::EvtNavigator();
    static TTimeStamp time(2014, 10, 18, 12, 15, 2, 111);
    time.Add(TTimeStamp(0, abs(m_r.Gaus(200000, 200000/5))));
    nav->setTimeStamp(time);

    // Put EvtNavigator into data buffer
    SniperPtr<IDataMemMgr> mMgr(getScope(), "BufferMemMgr");
    mMgr->adopt(nav, "/Event");

    // Create dummy event, set headers and events
    JM::PhyEvent* event = new JM::PhyEvent();
    event->setEnergy(m_r.Gaus(10,3));
    event->setRawEvis(m_r.Gaus(8,2));
    event->setEnrec(m_r.Gaus(12,3));
    event->setEprec(m_r.Gaus(12,3));
    JM::PhyHeader* header = new JM::PhyHeader();
    header->setEvent(event);

    // Put the header into EvtNavigator
    nav->addHeader("/Event/PhyEvent", header);

    LogInfo << "executing: " << m_iEvt
             << "  buffer_size: " << m_buf->size()
             << std::endl;

    return true;
    // After the executing of all algorithm of the task, current event in data
    // buffer will be saved, if output streams are configured. 
}

bool RootOutputTestAlg::finalize()
{
    LogInfo << " finalized successfully" << std::endl;

    return true;
}

