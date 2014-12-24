#include "MergeRootFilesAlg.h"
#include "SniperKernel/AlgFactory.h"

DECLARE_ALGORITHM(MergeRootFilesAlg);

MergeRootFilesAlg::MergeRootFilesAlg(const std::string& name)
    : AlgBase(name)
{
    declProp("InputFiles", m_inputFiles);
    declProp("OutputFile", m_outputFile)
}

MergeRootFilesAlg::~MergeRootFilesAlg()
{
}

bool MergeRootFilesAlg::initialize()
{
    IMerger* metaDataMerger = new FileMetaDataMerger;
    m_mergers.push_back(metaDataMerger);
    return true;
}

bool MergeRootFilesAlg::execute()
{
    return true;
}

bool MergeRootFilesAlg::finalize()
{
    for (std::vector<IMerger*>::iterator it = m_mergers.begin(); it != m_mergers.end(); ++it) {
        delete *it;
    }
    return true;
}
