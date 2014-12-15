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
        typedef std::vector<std::string> StringVector;

        FileMetaData() : m_NavPriority(-1) {}
        ~FileMetaData();

        // Add a TreeMetaData to this FileMetaData
        void AddTreeMetaData(JM::TreeMetaData* tmd);

        // Setter and Getter functions
        void SetNavPriority(int value) { m_NavPriority = value; }
        void SetNavPath(const StringVector& path) { m_NavPath = path; }
        void SetNavEventName(const StringVector& path) { m_NavEventName = path; }

        int GetNavPriority() const { return m_NavPriority; }
        TMDVector& GetTreeMetaData() { return m_TreeMetaDatas; }
        StringVector& GetNavPath() { return m_NavPath; }
        StringVector& GetNavEventName() { return m_NavEventName; }
        const TMDVector& GetTreeMetaData() const { return m_TreeMetaDatas; }
        const StringVector& GetNavPath() const { return m_NavPath; }
        const StringVector& GetNavEventName() const { return m_NavEventName; }

    private:
        StringVector m_NavPath;       // The data path holded by EvtNavigator in this file
        StringVector m_NavEventName;  // The name of event holded by EvtNavigator in this file
        TMDVector m_TreeMetaDatas;  // Container of TreeMetaData
        int m_NavPriority;          // Priority of this file

    ClassDef(FileMetaData,1)

};

} // namespace JM

#endif
