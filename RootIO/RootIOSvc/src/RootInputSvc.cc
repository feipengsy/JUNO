#include "RootIOSvc/RootInputSvc.h"
#include "RootIOSvc/RootInputStream.h"
#include "RootIOUtil/InputElementKeeper.h"
#include "RootIOUtil/RootFileReader.h"
#include "RootIOUtil/NavTreeList.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperPtr.h"

DECLARE_SERVICE(RootInputSvc);

RootInputSvc::RootInputSvc(const std::string& name) 
  : BaseIOSvc(name), m_fileReader(0),
    m_inputStream(0), m_keeper(0),
    m_regSvc(0)
{
    declProp("InputFile", m_inputFile);
}

RootInputSvc::~RootInputSvc()
{
}

bool RootInputSvc::initialize() 
{
    LogDebug << "Initialize RootInputSvc..." << std::endl;

    // Get element keeper and add its ref count by one
    m_keeper = InputElementKeeper::GetInputElementKeeper();
    m_keeper->AddRef();

    // Get the DataRegistritionSvc in current scope
    SniperPtr<DataRegistritionSvc> drs(this->getScope(), "DataRegistritionSvc");

    if (!drs.valid()) {
        LogError << "Fail to get DataRegistritionSvc instance"
                  << std::endl;
        return false;
    }

    m_regSvc = static_cast<DataRegistritionSvc*>(drs.data());
  
    // Construct file reader and input stream
    m_fileReader = new RootFileReader(m_keeper);
    m_inputStream = new RootInputStream(m_regSvc);


    // Add input files to the file reader
    if (!m_inputFile.size()) {
        LogError << "No input file set" 
                 << std::endl;
        return false;
    }
    std::vector<std::string>::iterator it, end = m_inputFile.end();

    LogDebug << "InputFile list: total " << m_inputFile.size()
             << std::endl;
    for (it = m_inputFile.begin(); it != end; ++it) {
        LogDebug <<  "Adding " << *it << std::endl;
        if(!m_fileReader->AddFile(*it)) {
          LogWarn << "Found reduplicated input file: " << *it
                  << ". Skipped" << std::endl;
        }
    }

    // Register input files to InputElementKeeper
    // and initialize input stream
    LogDebug << "Start registering input files to InputElementKeeper and initializing InputStream"
             << std::endl;
    NavTreeList* ntl = new NavTreeList();
    std::vector<std::string> paths, eventNames;
    if (!m_fileReader->ReadFiles(ntl, paths, eventNames)) {
        LogError << "Failed to register input file"
                 << std::endl;
        return false;
    }
    m_inputStream->registerTreeList(ntl);
    m_inputStream->registerNavPaths(paths, eventNames);

    LogDebug << "Sucessfully registered all input files, input stream initialized. " 
             << "RootInputSvc sucessfully initialized"
             << std::endl;
    return true;
}

bool RootInputSvc::finalize()
{
  LogDebug << "Finalizing RootInputSvc..."
           << std::endl;

  delete m_inputStream;
  delete m_fileReader;
  m_keeper->DecRef();

  LogDebug << "RootInputSvc sucessfully finalized"
           << std::endl;
  return true; 
}

RootInputStream* RootInputSvc::getInputStream()
{
  if (m_inputStream) {
      bool ok = m_inputStream->init();
      if (!ok) {
          LogError << "Fail to register path from input stream"
                   << std::endl;
          return 0;
      }
      return m_inputStream;
  }
  LogError << "InputStream not inialized"
           << std::endl;
  return 0;
}

bool RootInputSvc::getObj(TObject*& obj, const std::string& name, const std::string& path)
{
  TObject* readObj = 0;
  std::vector<int> fileList = m_keeper->GetFileList(path);
  if (0 == fileList.size()) {
      LogError << "Unknown path: " << path
               << ". Can't read object" << std::endl;
      return false;
  }
  readObj = m_fileReader->GetUserData(fileList, name);
  if (0 == readObj) {
      LogError << "Failed to read object: " << name 
               << std::endl;
      return false;
  }
  obj = readObj;
  return true;
}
