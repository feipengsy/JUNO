#ifndef ROOTIOUTIL_TREEMETADATA_H
#define ROOTIOUTIL_TREEMETADATA_H

#include "TObject.h"
#include <vector>
#include <map>
#include <string>

namespace JM {

class TreeMetaData : public TObject {

 public:
  typedef std::vector<std::vector<Int_t> > UIDVector;
  typedef std::vector<std::vector<Short_t> > BIDVector;
  typedef std::vector<std::string> GUIDVector;

  TreeMetaData() {}
  ~TreeMetaData() {}

  BIDVector& GetBranchIDs();
  UIDVector& GetUniqueIDs();
  GUIDVector& GetGUIDs();
  std::string& GetTreeName();

  void SetIDs(const UIDVector& uids, const BIDVector& bids);
  void SetGUIDs(const GUIDVector& guids);
  void SetTreeName(const std::string& name);

 private:
  UIDVector m_UniqueIDs;
  BIDVector m_BranchIDs;
  GUIDVector m_GUIDs;  
  std::string m_TreeName;

 ClassDef(TreeMetaData,1)

};

}

inline JM::TreeMetaData::BIDVector& JM::TreeMetaData::GetBranchIDs()
{
  return m_BranchIDs;
}

inline JM::TreeMetaData::UIDVector& JM::TreeMetaData::GetUniqueIDs()
{
  return m_UniqueIDs;
}

inline JM::TreeMetaData::GUIDVector& JM::TreeMetaData::GetGUIDs()
{
  return m_GUIDs;
}

inline void JM::TreeMetaData::SetIDs(const JM::TreeMetaData::UIDVector& uids, const JM::TreeMetaData::BIDVector& bids)
{
  m_UniqueIDs = uids;
  m_BranchIDs = bids;
}

inline void JM::TreeMetaData::SetGUIDs(const JM::TreeMetaData::GUIDVector& guids)
{
  m_GUIDs = guids;
}

inline void JM::TreeMetaData::SetTreeName(const std::string& name)
{
  m_TreeName = name;
}

inline std::string& JM::TreeMetaData::GetTreeName()
{
  return m_TreeName;
}

#endif
