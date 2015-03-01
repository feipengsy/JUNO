#ifndef ROOTIOTOOLS_MERGEROOTFILESALG_H
#define ROOTIOTOOLS_MERGEROOTFILESALG_H

#include "SniperKernel/AlgBase.h"
#include "IMerger.h"
#include <vector>
#include <string>

class TObject;
class TFile;
class TreeLooper;

class MergeRootFilesAlg : public AlgBase {

    public:
        MergeRootFilesAlg(const std::string* name);
        ~MergeRootFilesAlg();

        bool initialize();
        bool execute();
        bool finalize();

    private:
        bool rationalityCheck(std::vector<std::string>& dataPathList);
        void writeObj(TObject* obj, const std::string& path, const std::string& name);

    private:
        int                       m_index;
        std::vector<std::string>  m_inputFileNames
        std::string               m_outputFileName;
        std::vector<IMerger*>     m_objMergers;
        TFile*                    m_outputFile;
        TreeLooper*               m_treeLooper;

};


#endif
