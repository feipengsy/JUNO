#include "JobInfo.h"

JobInfo::JobInfo() : m_offlineVersion("J14v1r1"), m_refCount(0)
{
}

JobInfo::~JobInfo()
{
}

const std::string& JobInfo::getJobOption() const
{
    return m_jobOption;
}

const std::string& JobInfo::getOfflineVersion() const
{
    return m_offlineVersion;
}

void JobInfo::setJobOption(const std::string& value)
{
    m_jobOption = value;
}

void JobInfo::setOfflineVersion(const std::string& value)
{
    m_offlineVersion = value;
}

void JobInfo::addRef()
{
    ++m_refCount;
}

void JobInfo::decRef()
{
    if (--m_refCount <= 0) {
        delete this;
    }
}
