#ifndef ROOT_OUTPUT_TEST_ALG 
#define ROOT_OUTPUT_TEST_ALG

#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "TRandom.h"

class RootOutputTestAlg : public AlgBase
{
    public :
	
        RootOutputTestAlg(const std::string& name);
        virtual ~RootOutputTestAlg();

        virtual bool initialize();
        virtual bool execute();
        virtual bool finalize();

    private :

        JM::NavBuffer* m_buf;
        int m_iEvt;
        TRandom m_r;
};

#endif
