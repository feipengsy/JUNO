#include "RootIOSvc/RootInputSvc.h"
#include "RootIOSvc/RootInputStream.h"
#include "RootIOUtil/InputElementKeeper.h"
#include "RootIOUtil/RootFileReader.h"
#include "RootIOUtil/NavTreeList.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperPtr.h"

DECLARE_SERVICE(RootInputSvc);

RootInputSvc::RootInputSvc(const std::string& name) 
  : BaseIOSvc(name),
    m_inputStream(0), 
    m_keeper(0),
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

    // Print out input file list and erase reduplicated input files
    if (!m_inputFile.size()) {
        LogError << "No input file set"
                 << std::endl;
        return false;
    }
    std::vector<std::string>::iterator it, ret, beg = m_inputFile.begin();

    LogDebug << "InputFile list: total " << m_inputFile.size()
             << std::endl;

    LogDebug << "Input File: " << *beg << std::endl;
    for (it = beg + 1; it != m_inputFile.end(); ) {
        ret = std::find(beg, it, *it);
        if (ret != it) {
            LogWarn << "Found reduplicated input file: " << *it
                    << ". Skipped" << std::endl;
            it = m_inputFile.erase(it);
        }
        else {
            LogDebug << "Input File: " << *it << std::endl;
            ++it;
        }
    }
  
    // Construct input stream
    m_inputStream = new RootInputStream();

    // Register input files to InputElementKeeper
    // and initialize input stream
    LogDebug << "Start registering input files to InputElementKeeper and initializing InputStream"
             << std::endl;
    NavTreeList* ntl = new NavTreeList();
    std::vector<std::string> paths;
    if (!RootFileReader::ReadFiles(m_inputFile, ntl, paths)) {
        LogError << "Failed to register input file"
                 << std::endl;
        return false;
    }
    m_inputStream->registerTreeList(ntl);
    m_inputStream->registerNavPaths(paths);

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
  m_keeper->DecRef();

  LogDebug << "RootInputSvc sucessfully finalized"
           << std::endl;
  return true; 
}

RootInputStream* RootInputSvc::getInputStream()
{
    return m_inputStream;
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
  readObj = RootFileReader::GetUserData(fileList, name);
  if (0 == readObj) {
      LogError << "Failed to read object: " << name 
               << std::endl;
      return false;
  }
  obj = readObj;
  return true;
}
