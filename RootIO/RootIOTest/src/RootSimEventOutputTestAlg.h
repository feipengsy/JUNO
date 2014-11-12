#ifndef ROOT_SimEvent_OUTPUT_TEST_ALG 
#define ROOT_SimEvent_OUTPUT_TEST_ALG

#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "TRandom.h"

namespace JM {
    class SimEvent;
}

class RootSimEventOutputTestAlg : public AlgBase
{
    public :
        RootSimEventOutputTestAlg(const std::string& name);
        virtual ~RootSimEventOutputTestAlg();

        virtual bool initialize();
        virtual bool execute();
        virtual bool finalize();

    private:
        void generate_hits(JM::SimEvent*);
        void generate_trks(JM::SimEvent*);

    private :
        int m_totalpe;

        JM::NavBuffer* m_buf;
        int m_iEvt;
        TRandom m_r;
};

#endif
