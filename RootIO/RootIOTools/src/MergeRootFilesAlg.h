#ifndef ROOTIOTOOLS_MERGEROOTFILESALG_H
#define ROOTIOTOOLS_MERGEROOTFILESALG_H

#include "SniperKernel/AlgBase.h"
#include "IMerger.h"
#include <vector>
#include <string>

class MergeRootFilesAlg : public AlgBase {

    public:
        MergeRootFilesAlg(const std::string* name);
        ~MergeRootFilesAlg();

        bool initialize();
        bool execute();
        bool finalize();

    private:
        std::vector<std::string>  m_inputFiles;
        std::string               m_outputFile;
        std::vector<IMerger*>     m_merger;

};


#endif
