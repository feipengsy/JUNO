#ifndef ROOTIOUTIL_FILEMETADATA_H
#define ROOTIOUTIL_FILEMETADATA_H

#include "TObject.h"
#include "TreeMetaData.h"
#include <vector>
#include <map>

namespace JM {

class FileMetaData : public TObject {

    public:
        typedef std::vector<JM::TreeMetaData*> TMDVector;
        typedef std::vector<std::string> NameVector;

        FileMetaData() : m_NavPriority(-1) {}
        ~FileMetaData();

        // Add a TreeMetaData to this FileMetaData
        void AddTreeMetaData(JM::TreeMetaData* tmd);

        // Setter and Getter functions
        void SetNavPriority(int value);
        void SetNavPath(const std::vector<std::string>& path);
        void SetNavEventName(const std::vector<std::string>& path);

        int GetNavPriority() const;
        TMDVector& GetTreeMetaData();
        NameVector& GetNavPath();
        NameVector& GetNavEventName();
        const TMDVector& GetTreeMetaData() const;
        const NameVector& GetNavPath() const;
        const NameVector& GetNavEventName() const;

    private:
        NameVector m_NavPath;       // The data path holded by EvtNavigator in this file
        NameVector m_NavEventName;  // The name of event holded by EvtNavigator in this file
        TMDVector m_TreeMetaDatas;  // Container of TreeMetaData
        int m_NavPriority;          // Priority of this file

    ClassDef(FileMetaData,1)

};

} // namespace JM

inline void JM::FileMetaData::SetNavPriority(int value)
{
    m_NavPriority = value;
}

inline int JM::FileMetaData::GetNavPriority() const
{
    return m_NavPriority;
}

inline void JM::FileMetaData::AddTreeMetaData(JM::TreeMetaData* tmd)
{
    m_TreeMetaDatas.push_back(tmd);
}

inline void JM::FileMetaData::SetNavPath(const std::vector<std::string>& paths)
{
    m_NavPath = paths;
}

inline void JM::FileMetaData::SetNavEventName(const std::vector<std::string>& eventNames)
{
    m_NavEventName = eventNames;
}

inline JM::FileMetaData::TMDVector& JM::FileMetaData::GetTreeMetaData()
{
    return m_TreeMetaDatas;
}

inline const JM::FileMetaData::TMDVector& JM::FileMetaData::GetTreeMetaData() const
{
    return m_TreeMetaDatas;
}

inline JM::FileMetaData::NameVector& JM::FileMetaData::GetNavPath()
{
    return m_NavPath;
}

inline const JM::FileMetaData::NameVector& JM::FileMetaData::GetNavPath() const
{
    return m_NavPath;
}

inline JM::FileMetaData::NameVector& JM::FileMetaData::GetNavEventName()
{
    return m_NavEventName;
}

inline const JM::FileMetaData::NameVector& JM::FileMetaData::GetNavEventName() const
{
    return m_NavEventName;
}

#endif
