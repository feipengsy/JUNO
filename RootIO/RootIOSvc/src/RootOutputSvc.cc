#include "RootIOSvc/RootOutputSvc.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperPtr.h"
#include "RootIOSvc/RootOutputStream.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"

DECLARE_SERVICE(RootOutputSvc);

RootOutputSvc::RootOutputSvc(const std::string& name)
    : BaseIOSvc(name), m_regSvc(0), m_streamInitialized(false)
{
    declProp("OutputStreams", m_outputFileMap);
}

RootOutputSvc::~RootOutputSvc()
{
}

bool RootOutputSvc::initialize()
{
    LogDebug << "Initializing RootOutputSvc..."
             << std::endl;

    OutputFileMap::iterator it, end = m_outputFileMap.end();
    LogDebug << "Output streams list:"
             << std::endl;
    for (it = m_outputFileMap.begin(); it != end; ++it) {
        LogDebug << "Path: " << it->first << "  ==>  " 
                 << "File name: " << it->second
                 << std::endl;
    }

    LogDebug << "RootOutputSvc initialized."
             << std::endl;
    return true;
}

bool RootOutputSvc::finalize()
{
    LogDebug << "Finalizing RootOutputSvc..."
             << std::endl;
    OutputStreamVector::iterator it, end = m_outputStreams.end();
    for (it = m_outputStreams.begin(); it != end; ++it) {
        bool ok = (*it)->close();
        if (!ok) {
            LogError << "Error closing file for " << (*it)->path()
                     << std::endl;
            return false;
        }
    }
    LogDebug << "RootOutputSvc finalized."
             << std::endl;
    return true;
}

bool RootOutputSvc::initializeOutputStream()
{
    LogDebug << "Initializing RootOutputStreams..."
             << std::endl;

    // Get DataRegistritionSvc in current scope
    SniperPtr<DataRegistritionSvc> drs(this->getScope(), "DataRegistritionSvc");

    if (!drs.valid()) {
        LogError << "Fail to get DataRegistritionSvc instance"
                  << std::endl;
        return false;
    }

    m_regSvc = dynamic_cast<DataRegistritionSvc*>(drs.data());

    // Sort the paths according to the priority

    // The first string of the inner vector is the path of the stream to be generated.
    // Rest of the strings are the path of other streams share the same output file(if any).
    std::map<int, std::vector<std::string> > propaths;

    OutputFileMap::iterator it, it2, end  = m_outputFileMap.end();
    for (it = m_outputFileMap.begin(); it != end; ++it) {
        int priority = m_regSvc->getPriority(it->first);
        if (0 == priority) {
            LogError << "Found un-registered output path: " << it->first
                     << std::endl;
            return false;
        }
        std::vector<std::string> paths;
        paths.push_back(it->first);
        for (it2 = m_outputFileMap.begin(); it2 != end; ++it2) {
            // Don't add primary path again
            if (*it == *it2) { continue; }
            // Add the path shares the same output file
            if (it2->second == it->second) { paths.push_back(it2->first); }
        }
        // Map, should be automatically sorted.
        propaths.insert(std::make_pair(priority, paths));
    }

    
    // Now initialize the RootOutputStreams.
    std::map<int, std::vector<std::string> >::iterator pit, pend = propaths.end();
    for (pit = propaths.begin(); pit != pend; ++pit) {
        std::string primary_path = pit->second[0];
        std::string headerName = m_regSvc->getHeaderName(primary_path);
        std::string eventName = m_regSvc->getEventName(primary_path);
        std::map<std::string, int> otherPaths;
        if (pit->second.size() > 1) {
            // Have other paths
            std::vector<std::string>::iterator oit, oend = pit->second.end();
            for (oit = pit->second.begin() + 1; oit != oend; ++oit) {
                int opriority = m_regSvc->getPriority(*oit);
                otherPaths.insert(std::make_pair(*oit, opriority));
            }
        }
        RootOutputStream* stream = new RootOutputStream(headerName, eventName, primary_path, pit->first, otherPaths, m_regSvc);
        // Start the output file
        stream->newFile(m_outputFileMap[primary_path]);
        // Then the vector is sorted according to priotity
        m_outputStreams.push_back(stream);
    }

    OutputObjMap::iterator eit, eend = m_exOutputObjs.end();
    LogDebug << "+ m_exOutputObjs size: " << m_exOutputObjs.size() << std::endl;
    for (eit = m_exOutputObjs.begin(); eit != eend; ++eit) {
        LogDebug << "Attach Obj: " << eit->first << std::endl;
        doAttachObj(eit->first, eit->second);
    }
    m_streamInitialized = true;
    LogDebug << "All RootOutputStreams are Initialized."
             << std::endl;
    return true;
}

bool RootOutputSvc::write(JM::EvtNavigator* nav)
{
    if (!nav) {
        LogError << "No EvtNavigator, can not write"
                 << std::endl;
        return false;
    }

    LogDebug << "Writing data to output files..."
             << std::endl;

    OutputStreamVector::iterator it, end = m_outputStreams.end();
    for (it = m_outputStreams.begin(); it != end; ++it) {
        bool write = nav->writePath((*it)->path());
        if (!write) {
            LogDebug << "Skipping path: " << (*it)->path()
                     << std::endl;
            continue;
        }
        LogDebug << "Writing path: " << (*it)->path()
                 << std::endl;
        LogDebug << "(*it)->setAddress(nav)" << std::endl;
        bool ok = (*it)->setAddress(nav);
        if (!ok) {
            LogError << "Fail to set address for path: " << (*it)->path()
                     << std::endl;
            return false;
        }
        LogDebug << "(*it)->write()" << std::endl;
        ok = (*it)->write();
        if (!ok) {
            LogError << "Fail to write stream for path: " << (*it)->path()
                     << std::endl;
            return false;
        }
    }
    LogDebug << "Finish write(JM::EvtNavigator*)" << std::endl;
    return true;
}

bool RootOutputSvc::attachObj(const std::string& path, TObject* obj) 
{
    std::pair<OutputObjMap::iterator, OutputObjMap::iterator> pos = m_exOutputObjs.equal_range(path);
    OutputObjMap::iterator it;
    for (it = pos.first; it != pos.second; ++it) {
        if (it->second == obj) {
            LogError << "Found duplicated object, failed to attach"
                     << std::endl;
            return false;
        }
    }
    m_exOutputObjs.insert(std::make_pair<std::string, TObject*>(path,obj));
    LogDebug << "Attach Obj: " << path << std::endl;
    LogDebug << "+ m_exOutputObjs size: " << m_exOutputObjs.size() << std::endl;
    if (m_streamInitialized) {
        return doAttachObj(path, obj);
    }
    return true;
}

bool RootOutputSvc::doAttachObj(const std::string& path, TObject* obj)
{
    OutputStreamVector::iterator it, end = m_outputStreams.end();
    for (it = m_outputStreams.begin();it != end;++it) {
        LogDebug << "* Path: " << (*it)->path() << std::endl;
        if (path == (*it)->path()) {
            // Hit
            bool ok = (*it)->attachObj(obj);
            if (!ok) {
                LogError << "Failed to attach " << obj->ClassName()
                         << std::endl;
            }
            return ok;
        }
    }
    // Miss
    LogError << "Can not find output stream: " << path 
             << ". Failed to attach " << obj->ClassName()
             << std::endl;
    return false;
}
