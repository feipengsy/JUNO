#ifndef ROOT_MULTI_TASK_TEST_ALG 
#define ROOT_MULTI_TASK_TEST_ALG

#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"

class RootMultiTaskTestAlg : public AlgBase
{
    public :

        RootMultiTaskTestAlg(const std::string& name);
        virtual ~RootMultiTaskTestAlg();

        virtual bool initialize();
        virtual bool execute();
        virtual bool finalize();

    private :

        int m_count;
};

#endif
