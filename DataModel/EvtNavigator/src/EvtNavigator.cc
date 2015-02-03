#include "EvtNavigator/EvtNavigator.h"
#include "EDMManager.h"
#include <algorithm>

ClassImp(JM::EvtNavigator);

JM::EvtNavigator::~EvtNavigator()
{
    std::vector<JM::SmartRef*>::iterator it, end = m_refs.end();
    for (it = m_refs.begin(); it != end; ++it) {
        delete *it;
    }
}

JM::EvtNavigator::EvtNavigator(const JM::EvtNavigator& nav)
    : TObject(nav)
{
    init(nav);
}

JM::EvtNavigator& JM::EvtNavigator::operator=(const JM::EvtNavigator& nav)
{
    if (this != &nav) {
        TObject::operator=(nav);
        init(nav);
    }
    return *this;
}

void JM::EvtNavigator::init(const JM::EvtNavigator& nav)
{
    m_paths = nav.m_paths;
    m_writeFlag = nav.m_writeFlag;
    m_TimeStamp = nav.m_TimeStamp;
    // Clear previous SmartRefs
    std::vector<SmartRef*>::iterator it, end = m_refs.end();
    for (it = m_refs.begin(); it != end; ++it) {
        delete *it;
    }
    // Copy new SmartRefs
    std::vector<SmartRef*>::const_iterator it2, end2 = nav.m_refs.end();
    for (it2 = nav.m_refs.begin(); it2 != end2; ++it2) {
        SmartRef* ref = new SmartRef(**it2);
        m_refs.push_back(ref);
    }
}

JM::HeaderObject* JM::EvtNavigator::getHeader(const std::string& path)
{
    std::vector<std::string>::iterator pos = find(m_paths.begin(), m_paths.end(), path);
    if (m_paths.end() == pos) return 0;
    m_refs[pos - m_paths.begin()]->SetBranchID(0);
    return static_cast<JM::HeaderObject*>(m_refs[pos - m_paths.begin()]->GetObject());
}

std::vector<std::string>& JM::EvtNavigator::getPath()
{
    return m_paths;
}

const std::vector<std::string>& JM::EvtNavigator::getPath() const
{
    return m_paths;
}

std::vector<JM::SmartRef*>& JM::EvtNavigator::getRef()
{
    return m_refs;
}

const std::vector<JM::SmartRef*>& JM::EvtNavigator::getRef() const
{   
    return m_refs;
}

void JM::EvtNavigator::setHeaderEntry(const std::string& path, int entry)
{
    std::vector<std::string>::iterator pos = find(m_paths.begin(), m_paths.end(), path);
    if (m_paths.end() == pos) return;
    m_refs[pos - m_paths.begin()]->setEntry(entry);
}

void JM::EvtNavigator::addHeader(const std::string& path, JM::HeaderObject* header)
{
    m_paths.push_back(path);
    SmartRef* ref = new SmartRef();
    m_refs.push_back(ref);
    m_refs.back() -> SetObject(header);
    m_writeFlag.push_back(true);
}

void JM::EvtNavigator::addHeader(JM::HeaderObject* header) 
{
    m_paths.push_back(EDMManager::get()->getPathWithHeader(header->ClassName()));
    SmartRef* ref = new SmartRef();
    m_refs.push_back(ref);
    m_refs.back()->SetObject(header);
    m_writeFlag.push_back(true);
}

void JM::EvtNavigator::setPath(const std::vector<std::string>& paths)
{
    m_paths = paths;
}

bool JM::EvtNavigator::writePath(const std::string& path)
{
    std::vector<std::string>::iterator pos = find(m_paths.begin(), m_paths.end(), path);
    if (m_paths.end() == pos) return false;
    return m_writeFlag[pos - m_paths.begin()];
}

void JM::EvtNavigator::setWriteFlag(const std::string& path, bool write)
{
    std::vector<std::string>::iterator pos = find(m_paths.begin(), m_paths.end(), path);
    if (m_paths.end() == pos) return;
    m_writeFlag[pos - m_paths.begin()] = write;
}

void JM::EvtNavigator::resetWriteFlag()
{
    if (m_writeFlag.size()) m_writeFlag.clear();
    int npath = m_paths.size();
    for (int i = 0; i < npath; ++i) {
        m_writeFlag.push_back(true);
    }
}

const TTimeStamp& JM::EvtNavigator::TimeStamp() const
{
  return m_TimeStamp;
}

TTimeStamp& JM::EvtNavigator::TimeStamp()
{
  return m_TimeStamp;
}

void JM::EvtNavigator::setTimeStamp(const TTimeStamp& value)
{
  m_TimeStamp = value;
}

