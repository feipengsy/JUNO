#include "RootIOSvc/RootInputSvc.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperPtr.h"
#include <boost/foreach.hpp>

DECLARE_SERVICE(RootInputSvc);

RootInputSvc::RootInputSvc(const std::string& name) 
  : SvcBase(name)
{
    declProp("InputFile",  m_navInputFile);
    declProp("InputFileMap", m_inputFileMap);
}

RootInputSvc::~RootInputSvc()
{
}

bool RootInputSvc::initialize() 
{
    LogInfo << "Initialize RootInputSvc..." << std::endl;

    // initialize input stream(s)
    bool okay;
    if (!m_inputFile.size()) {
        okay = this->initPlainStream();
    }
    else {
        okay = this->initNavStream();
    }

    if (!okay) {
        LogError << "Failed to initialize RootInputSvc." 
                 << std::endl;
        return false;
    }

    LogInfo << "RootInputSvc successfully initialized." 
            << std::endl;
    return true;
}

bool RootInputSvc::finalize()
{
  LogInfo << "Finalizing RootInputSvc..."
          << std::endl;
 
  bool okay = true;
  BOOST_FOREACH(InputStreamMap::value_type& item, m_inputStream) {
      okay = okay && item.second->finalize();
      delete item.second;
  }

  if (!okay) {
      LogError << "One or more input streams is failed to finalzie, please check!"
               << std::endl;
      return false;
  }

  LogInfo << "RootInputSvc sucessfully finalized"
          << std::endl;
  return true; 
}

IInputStream* RootInputSvc::getInputStream(const std::string& path)
{
    InputStreamMap::iterator pos = m_inputStream.find(path);
    if (pos == m_inputStream.end()) {
        return 0;
    }
    return pos->second;
}

bool RootInputSvc::getObj(TObject*& obj, const std::string& name, const std::string& path)
{
    std::string objName;
    InputStreamMap::iterator streamPos = m_inputStream.find("EvtNavigator");
    if (streamPos == m_inputStream.end()) {
        // NavInputStream is not managed
        if (path == "none") {
            LogError << "Path not provided, can not read object: " << name
                     << std::endl;
            return false;
        }   
        pos = m_inputStream.find(path);
        if (pos == m_inputStream.end()) {
            LogError << "Failed to find path: " << path
                     << ". Can not read object: " << name
                     << std::endl;
            return false;
        }
        objName = name;
    }
    else {
        // For NavInputStream, all paths are managed together, so path need to be provided.
        objName = path + name;
    }

    // Stream found, start to get object
    bool okay = pos->second->getObj(obj, objName);
    if (!okay) {
        LogError << "Failed to read object: " << name
                 << std::endl;
    } 
    return okay;
}

bool RootInputSvc::initPlainStream()
{
    BOOST_FOREACH(const InputFileMap::value_type& item, m_inputFileMap) {
        LogDebug << "Creating RootInputStream of: " << item.first
                 << std::endl;
        IInputStream* stream = new RootInputStream(item.second);
        bool okay = stream->initialize();
        if (!okay) {
            LogError << "Failed to initialize RootInputStream of: " << item.first
                     << std::endl;
            return false;
        }
        m_inputStream.push_back(std::make_pair(item.first, stream));
    }
    return true;
}


bool RootInputSvc::initNavStream()
{
    LogDebug << "Creating NavInputStream..." << std::endl;

    // Construct input file list
    std::vector<std::string> files = m_navInputFile;
    BOOST_FOREACH(const InputFileMap::value_type& item, m_inputFileMap) {
        // Should add all other input files user configured(for event navigating)
        files.insert(files.end(), item.second.first(), item.second.end());
    }
    IInputStream* stream = new NavInputStream(files);
    bool okay = stream->initialize();
    if (!okay) {
        LogError << "Failed to initialize NavInputStream"
                 << std::endl;
        return false;
    }

    m_inputStream.push_back(std::make_pair("EvtNavigator", stream));
    return true;
}
