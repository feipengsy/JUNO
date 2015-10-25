#include "FileMetaData.h"

ClassImp(JM::FileMetaData);

JM::FileMetaData::FileMetaData(const JM::FileMetaData& fmd)
     : TObject(fmd), m_NavPath(fmd.m_NavPath), m_UUIDList(fmd.m_UUIDList), m_NavPriority(fmd.m_NavPriority)
{
    for(TMDVector::const_iterator it = fmd.m_TreeMetaDatas.begin(); it != fmd.m_TreeMetaDatas.end(); ++it) {
        m_TreeMetaDatas.push_back(new JM::TreeMetaData(**it));
    }
}

JM::FileMetaData::~FileMetaData()
{
    for(TMDVector::iterator it = m_TreeMetaDatas.begin(); it != m_TreeMetaDatas.end(); ++it) {
        delete *it;
    }
}

bool JM::FileMetaData::IsSameAs(const JM::FileMetaData* other)
{
    bool similar = m_NavPath == other->m_NavPath &&\
                   m_NavPriority == other->m_NavPriority &&\
                   m_TreeMetaDatas.size() == other->m_TreeMetaDatas.size() ;
    if (!similar) {
        return false;
    }
    for (size_t i = 0; i < m_TreeMetaDatas.size(); ++i) {
        similar = m_TreeMetaDatas[i]->IsSameAs(other->m_TreeMetaDatas[i]);
        if (!similar) {
            return false;
        }
    }
    return true;
}
