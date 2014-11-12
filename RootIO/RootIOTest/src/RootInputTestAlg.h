#ifndef ROOT_INPUT_TEST_ALG 
#define ROOT_INPUT_TEST_ALG

#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"

class RootInputTestAlg : public AlgBase
{
    public :

        RootInputTestAlg(const std::string& name);
        virtual ~RootInputTestAlg();

        virtual bool initialize();
        virtual bool execute();
        virtual bool finalize();

    private :

        int m_iEvt;
        JM::NavBuffer* m_buf;
};

#endif
