#include "RootIOUtil/FileMetaData.h"

ClassImp(JM::FileMetaData);

JM::FileMetaData::FileMetaData(const JM::FileMetaData& fmd)
     : TObject(fmd), m_NavPath(fmd.m_NavPath), m_UUIDList(fmd.m_UUIDList), m_NavPriority(fmd.m_NavPriority)
{
    for(TMDVector::iterator it = fmd.m_TreeMetaDatas.begin(); it != fmd.m_TreeMetaDatas.end(); ++it) {
        m_TreeMetaDatas.push_back(new JM::TreeMetaData(&*it));
    }
}

JM::FileMetaData::~FileMetaData()
{
    for(TMDVector::iterator it = m_TreeMetaDatas.begin(); it != m_TreeMetaDatas.end(); ++it) {
        delete *it;
    }
}
