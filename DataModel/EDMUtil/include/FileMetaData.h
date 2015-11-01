#ifndef FILE_META_DATA_H
#define FILE_META_DATA_H 0

#include "TObject.h"
#include <vector>
#include <map>

namespace JM {

class TreeMetaData : public TObject {

    public:
        TreeMetaData() {}
        ~TreeMetaData() {}

        bool IsSameAs(const TreeMetaData* other) { return m_TreeName == other->m_TreeName && m_ObjName == other->m_ObjName; }
        std::string& GetTreeName() { return m_TreeName; }
        std::string& GetObjName() { return m_ObjName; }
        const std::string& GetObjName() const { return m_ObjName; }
        const std::string& GetTreeName() const { return m_TreeName; }
        void SetTreeName(const std::string& value) { m_TreeName = value; }
        void SetObjName(const std::string& value) { m_ObjName = value; }

    private:
        std::string m_TreeName;  // Tree name(path) of this tree
        std::string m_ObjName;   // Name of object holded by this tree

    ClassDef(TreeMetaData,1)

};

class FileMetaData : public TObject {

    public:
        typedef std::vector<JM::TreeMetaData*> TMDVector;
        typedef std::vector<std::string> StringVector;
        typedef std::map<std::string, std::vector<Long64_t> > BreakPointsMap;

        FileMetaData() : m_NavPriority(-1) {}
        FileMetaData(const FileMetaData& fmd);
        ~FileMetaData();

        // Add a TreeMetaData to this FileMetaData
        void AddTreeMetaData(JM::TreeMetaData* tmd) { m_TreeMetaDatas.push_back(tmd); }
        // Test wether another FileMetaData is same(capable of merging)
        bool IsSameAs(const JM::FileMetaData* other);
        // Setter and Getter functions
        void SetNavPriority(int value) { m_NavPriority = value; }
        void SetNavPath(const StringVector& path) { m_NavPath = path; }
        void SetUUIDList(const StringVector& uuids) { m_UUIDList = uuids; }
        void SetBreakPoints(const BreakPointsMap& breakPoints) { m_BreakPoints = breakPoints; }

        int GetNavPriority() const { return m_NavPriority; }
        TMDVector& GetTreeMetaData() { return m_TreeMetaDatas; }
        StringVector& GetNavPath() { return m_NavPath; }
        StringVector& GetUUIDList() { return m_UUIDList; }
        BreakPointsMap& GetBreakPoints() { return m_BreakPoints; }
        const TMDVector& GetTreeMetaData() const { return m_TreeMetaDatas; }
        const StringVector& GetNavPath() const { return m_NavPath; }
        const StringVector& GetUUIDList() const { return m_UUIDList; }
        const BreakPointsMap& GetBreakPoints() const { return m_BreakPoints; }

    private:
        StringVector m_NavPath;       // The data path holded by EvtNavigator in this file
        StringVector m_UUIDList;      // The list of TProcessID uuids holded by objects in this file
        BreakPointsMap m_BreakPoints; // The map of break points(because of merging of trees)
        TMDVector m_TreeMetaDatas;    // Container of TreeMetaData
        int m_NavPriority;            // Priority of this file

    ClassDef(FileMetaData,1)

};

} // namespace JM

#endif
