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

  void SetNavPriority(int value);
  void SetNavPath(const std::vector<std::string>& path);
  void SetNavEventName(const std::vector<std::string>& path);
  int GetNavPriority() const;
  void AddTreeMetaData(JM::TreeMetaData* tmd);
  TMDVector& GetTreeMetaData();
  const TMDVector& GetTreeMetaData() const;
  NameVector& GetNavPath();
  const NameVector& GetNavPath() const;
  NameVector& GetNavEventName();
  const NameVector& GetNavEventName() const;


 private:
  NameVector m_NavPath;
  NameVector m_NavEventName;
  TMDVector m_TreeMetaDatas;
  int m_NavPriority;

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
