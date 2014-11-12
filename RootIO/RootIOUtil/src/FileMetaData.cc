#include "RootIOUtil/FileMetaData.h"

ClassImp(JM::FileMetaData);

JM::FileMetaData::~FileMetaData()
{
    for(TMDVector::iterator it = m_TreeMetaDatas.begin(); it != m_TreeMetaDatas.end(); ++it) {
        delete *it;
    }
}
