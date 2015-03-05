#include "RootIOUtil/TreeMetaData.h"

ClassImp(JM::TreeMetaData)

bool JM::TreeMetaData::IsSameAs(const JM::TreeMetaData* other)
{
    return m_TreeName == other->m_TreeName && m_ObjName == other->m_ObjName;
}

