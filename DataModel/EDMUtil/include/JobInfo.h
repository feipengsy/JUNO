#ifndef EDMUTIL_JOBINFO_H
#define EDMUTIL_JOBINFO_H

#include "TObject.h"

#include <string>

class JobInfo : public TObject {
    public:
        JobInfo();
        ~JobInfo();

        const std::string& getJobOption() const;
        const std::string& getOfflineVersion() const;
        void setJobOption(const std::string& value);
        void setOfflineVersion(const std::string& value);

        // Reference count
        void addRef();
        void decRef();

    private:
        std::string m_jobOption;
        std::string m_offlineVersion;
        int m_refCount; //!

    ClassDef(JobInfo,1);
};

#endif
