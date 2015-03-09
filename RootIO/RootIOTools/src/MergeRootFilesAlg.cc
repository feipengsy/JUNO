#include "MergeRootFilesAlg.h"
#include "TreeLooper.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/Incident.h"
#include "FileMetaDataMerger.h"
#include "UniqueIDTableMerger.h"
#include "RootIOUtil/FileMetaData.h"
#include "RootIOUtil/TreeMetaData.h"
#include "RootIOUtil/RootFileReader.h"

#include "TObject.h"
#include "TFile.h"

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
    m_outputFile = new TFile(m_outputFileName.c_str(), "recreate");

    // Create all the mergers
    IMerger* metaDataMerger = new FileMetaDataMerger(&m_mdBreakPoints);
    IMerger* uidMerger = new UniqueIDTableMerger();
    m_objMergers.push_back(metaDataMerger);
    m_objMergers.push_back(uidMerger);
    m_treeLooper = new TreeLooper(dataPathMap, m_outputFile);

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
    // Finalize tree looper
    m_treeLooper->finalize();
    m_mdBreakPoints = m_treeLooper->getBreakPoints();

    // Merge the additionally TObjects
    std::vector<IMerger*>::iterator it;
    TObject* obj = 0;
    for (it = m_objMergers.begin(); it != m_objMergers.end(); ++it) {
        (*it)->setInputFile(m_inputFileNames);
        (*it)->setOutputFile(m_outputFileName);
        std::string path, name;
        (*it)->merge(obj, path, name);
        this->writeObj(obj, path, name);
    }

    for (it = m_objMergers.begin(); it != m_objMergers.end(); ++it) {
        delete *it;
    }
    delete m_treeLooper;
    return true;
}

bool MergeRootFilesAlg::rationalityCheck(PathMap& dataPathMap)
{
    // Check the input file list to see if they were capable of being merged.
    if (m_inputFileNames.size() <= 1) {
        LogError << "No input file or just one input file specified, can not merge." << std::endl;
        return false;
    }
    std::vector<std::string>::iterator it, end = m_inputFileNames.end();
    JM::FileMetaData* firstFmd = 0;
    JM::FileMetaData* fmd = 0;
    for (it = m_inputFileNames.begin(); it != end; ++it) {
        TFile* file = new TFile(it->c_str(), "read");
        if (!file->IsOpen()) {
            LogError << "Failed to open file: " << *it << std::endl;
            delete file;
            return false;
        }
        fmd = RootFileReader::GetFileMetaData(file);
        file->Close();
        delete file;
        if (!fmd) {
            LogError << "Failed to read file meta data from file: " << *it << std::endl;
            return false;
        }
        if (!firstFmd) {
            // First file, made as reference
            firstFmd = fmd;
            continue;
        }
        if (!firstFmd->IsSameAs(fmd)) {
            LogError << "File: " << *it << " is not same, can not merge" << std::endl;
            delete fmd;
            return false;
        }
        delete fmd;
    } 
    const JM::FileMetaData::TMDVector& tmds = firstFmd->GetTreeMetaData();
    JM::FileMetaData::TMDVector::const_iterator tit, tend = tmds.end();
    for (tit = tmds.begin(); tit != tend; ++tit) {
        dataPathMap.insert(std::make_pair((*tit)->GetTreeName(),(*tit)->GetObjName()));
    }
    delete firstFmd;
    return true;
}

void MergeRootFilesAlg::writeObj(TObject* obj, const std::string& path, const std::string& name)
{
    m_outputFile->cd();
    if (!gDirectory->cd(path.c_str())) {
        gDirectory->mkdir(path.c_str());
        gDirectory->cd(path.c_str());
    }
    obj->Write(name.c_str());
    delete obj;
}
