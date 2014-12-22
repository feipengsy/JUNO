#include "RootIOSvc/RootOutputSvc.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperPtr.h"
#include "RootIOSvc/RootOutputStream.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "RootIOUtil/RootOutputFileManager.h"

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

    String2String::iterator it, end = m_outputFileMap.end();
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
    // Do nothing, just for old version compatibility
    // Output streams will be initialized by the initalizeOutputStream(JM::EvtNavigator*) version
    return true;
}

bool RootOutputSvc::initializeOutputStream(JM::EvtNavigator* nav)
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

    // Now, try to confirm the event type of output paths
    for (String2String::iterator it = m_outputFileMap.begin(); it != m_outputFileMap.end(); ++it) {
        JM::HeaderObject* header = nav->getHeader(*it);
        if (header) {
            m_path2typeMap.insert(std::make_pair(*it, header->ClassName()));
        }
        else {
            // The EvtNavigator does not hold this output path
            m_path2typeMap.insert(std::make_pair<std::string, std::string>(*it, "unknown"));
            LogWarn << "Can not find path: " << *it 
                    << "Skipped for now" << std::endl;
            m_notYetInitPaths.push_back(*it);
        }
    }
    
    // Sort the paths according to the priority

    // The first string of the inner vector is the path of the stream to be generated.
    // Rest of the strings are the path of other streams share the same output file(if any).
    std::multimap<int, std::vector<std::string> > priority2paths;

    String2String::iterator it, it2, end  = m_outputFileMap.end();
    for (it = m_outputFileMap.begin(); it != end; ++it) {
        // Get priority of this path, get 0 out of a "unknown" path
        int priority = EDMManager::get()->getPriorityWithHeader(m_path2typeMap[it->first]);
        std::vector<std::string> paths;
        paths.push_back(it->first);
        for (it2 = m_outputFileMap.begin(); it2 != end; ++it2) {
            // Don't add primary path again
            if (*it == *it2) { continue; }
            // Add the path shares the same output file
            if (it2->second == it->second) { paths.push_back(it2->first); }
        }
        // Map, should be automatically sorted.
        priority2paths.insert(std::make_pair(priority, paths));
    }

    // Now create output file and initialize RootOutputStreams.
    std::multimap<int, std::vector<std::string> >::iterator pit, pend = priority2paths.end();
    for (pit = priority2paths.begin(); pit != pend; ++pit) {
        // Create output file
        std::map<std::string, int> path2priority;
        std::vector<std::string>::iterator oit, oend = pit->second.end();
        for (oit = pit->second.begin(); oit != oend; ++oit) {
            int opriority = EDMManager::get()->getPriorityWithHeader(m_path2typeMap[*oit]);
            path2priority.insert(std::make_pair(*oit, opriority));
        }
        std::string primary_path = pit->second[0];
        // RootOutputFileManager won't create dupilicated output files
        RootOutputFileManager::get()->new_file(m_outputFileMap[primary_path], path2priority);
        // Create output stream
        std::string headerName = m_path2typeMap[*oit];
        std::string eventName = EDMManager::get()->getEventNameWithHeader(m_path2typeMap[*oit]);
        // Maybe regSvc is no longer needed
        RootOutputStream* stream = new RootOutputStream(headerName, eventName, primary_path, m_regSvc);
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

    if (!m_streamInitialized) {
        // Output streams are not initialized yet
        bool ok = initializeOutputStream(nav);
        if (!ok) {
            LogError << "Fail to initialize output streams" 
                     << std::endl;
            return false;
        }
    }

    // Do we need to initialize the not yet initialized streams?
    StringVector::iterator nit, nend = m_notYetInitPaths.end();
    for (nit = m_notYetInitPaths.begin(); nit != nend; ++nit) {
        JM::HeaderObject header = nav->getHeader(nit);
        if (header) {
            // Now we have got the "unknown" path, revise the corresponing output stream
            bool ok = reviseOutputStream(*it, header->EventName());
            if (!ok) {
                LogError << "Fail to re-initialize output streams"
                         << std::endl;
                return false;
            }
        }
    }

    LogDebug << "Writing data to output files..."
             << std::endl;

    OutputStreamVector::iterator it, end = m_outputStreams.end();
    for (it = m_outputStreams.begin(); it != end; ++it) {
        bool write = nav->writePath((*it)->path());
        if (!write) {
            // TODO can not just skip it
            LogDebug << "Skipping path: " << (*it)->path()
                     << std::endl;
            continue;
        }
        LogDebug << "Writing path: " << (*it)->path()
                 << std::endl;
        // Set address
        (*it)->setAddress(nav);
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

bool RootOutputSvc::reviseOutputStream(const std::string& path, const std::string& headerName)
{
    OutputStreamVector::iterator it, end = m_outputStreams.end();
    for (it = m_outputStreams.begin(); it != end; ++it) {
        if (path == (*it)->path()) {
            // Reset the header and event name of this stream
            (*it)->setHeaderName(headerName);
            (*it)->setEventName(EDMManager::get()->getEventNameWithHeader(headerName));
            // Notify the output file
            int priority = EDMManager::get()->getPriorityWithHeader(headerName);
            RootOutputFileManager::get()->reviseOutputFile(m_outputFileMap[path], path, priority);
        }
    }
    // Notify other output streams
    for (it = m_outputStreams.begin(); it != end; ++it) {
        if (path == (*it)->path()) {
            continue;
        }
        (*it)->revise();
    }
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
    String2String::iterator pos = m_outputFileMap.find(path);
    if (pos != m_outputFileMap.end()) {
        RootOutputFileHandle* file = RootOutputFileManager::get()->get_file_with_name(it->second);
        if (strcmp(obj->ClassName(), "TGeoManager") == 0) {
            file->addGeoManager(static_cast<TGeoManager*>(obj));
            return true;
        }
        // TODO Other object type... 
        return true;
    }
    // Miss
    LogError << "Can not find output stream: " << path 
             << ". Failed to attach " << obj->ClassName()
             << std::endl;
    return false;
}
