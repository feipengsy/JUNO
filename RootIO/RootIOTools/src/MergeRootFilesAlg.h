#ifndef ROOTIOTOOLS_MERGEROOTFILESALG_H
#define ROOTIOTOOLS_MERGEROOTFILESALG_H

#include "SniperKernel/AlgBase.h"
#include "IMerger.h"
#include "TObject.h"
#include <vector>
#include <string>

class TFile;
class TreeLooper;

class MergeRootFilesAlg : public AlgBase {

    public:
        typedef std::map<std::string, std::string> PathMap; // Data path to object name
        typedef std::map<std::string, std::vector<Long64_t> > BreakPointsMap;
        MergeRootFilesAlg(const std::string& name);
        ~MergeRootFilesAlg();

        bool initialize();
        bool execute();
        bool finalize();

    private:
        bool rationalityCheck(PathMap& dataPathMap);
        void writeObj(TObject* obj, const std::string& path, const std::string& name);

    private:
        unsigned int              m_index;
        std::vector<std::string>  m_inputFileNames;
        std::string               m_outputFileName;
        std::vector<IMerger*>     m_objMergers;
        TFile*                    m_outputFile;
        TreeLooper*               m_treeLooper;
        BreakPointsMap            m_mdBreakPoints;

};


#endif
