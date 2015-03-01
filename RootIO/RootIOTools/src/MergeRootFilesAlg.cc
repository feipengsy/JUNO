#include "MergeRootFilesAlg.h"
#include "TreeLooper.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"

#include "TObject.h"

DECLARE_ALGORITHM(MergeRootFilesAlg);

MergeRootFilesAlg::MergeRootFilesAlg(const std::string& name)
    : AlgBase(name)
    , m_index(0)
    , m_outputFile(0)
    , m_treeLooper(0)
{
    declProp("InputFiles", m_inputFileNames);
    declProp("OutputFile", m_outputFileName);
}

MergeRootFilesAlg::~MergeRootFilesAlg()
{
}

bool MergeRootFilesAlg::initialize()
{
    // Reasonableness check
    PathMap dataPathMap;
    bool ok = this->rationalityCheck(dataPathMap);
    if (!ok) {
        LogError << "Failed on rationality check, can not merge. Please check input files."
                 << std::endl;
        return false;
    }

    // Create output file
    m_outputFile = new TFile(m_outputFileName, "recreate");

    // Create all the mergers
    IMerger* metaDataMerger = new FileMetaDataMerger;
    IMerger* uidMerger = new UniqueIDTableMerger;
    m_objMergers.push_back(metaDataMerger);
    m_objMergers.push_back(uidMerger);
    m_treeLooper = new TreeLooper(dataPathMap);
    m_treeLooper->initialize();

    LogInfo << "Successfully initialized!" << std::endl;
    return true;
}

bool MergeRootFilesAlg::execute()
{
    bool ok = m_treeLooper->next();
    if (!ok) {
        if (m_index < m_inputFileNames.size()) {
            // Start a new file
            m_treeLooper->newInputFile(m_inputFileNames[m_index++]);
        }
        else {
            Incident::fire("StopRun");
        }
    }
    return true;
}

bool MergeRootFilesAlg::finalize()
{
    // Merge the additionally TObjects
    std::vector<IMerger*>::iterator it;
    TObject* obj = 0;
    for (it = m_objMergers.begin(); it != m_objMergers.end(); ++it) {
        (*it)->setInputFile(m_inputFileNames);
        std::string path, name;
        (*it)->merge(obj, path, name);
        this->writeObj(obj, path, name);
    }

    // finalize
    m_treeLooper->finalize();
    for (it = m_objMergers.begin(); it != m_objMergers.end(); ++it) {
        delete *it;
    }
    delete m_treeLooper;
    return true;
}

bool MergeRootFilesAlg::rationalityCheck(PathMap& dataPathMap)
{
    return true;
}

void MergeRootFilesAlg::writeObj(TObject* obj, const std::string& path, const std::string& name)
{
    m_outputFile->cd();
    if (!gDirectory->cd(path)) {
        gDirectory->mkdir("path");
        gDirectory->cd(path);
    }
    obj->Write(name);
}
