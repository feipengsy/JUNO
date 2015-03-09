#ifndef ROOTIOUTIL_TREEMETADATA_H
#define ROOTIOUTIL_TREEMETADATA_H

#include "TObject.h"
#include <string>

namespace JM {

class TreeMetaData : public TObject {

    public:
        TreeMetaData() {}
        ~TreeMetaData() {}

        bool IsSameAs(const TreeMetaData* other);
        std::string& GetTreeName() { return m_TreeName; }
        std::string& GetObjName() { return m_ObjName; }
        const std::string& GetObjName() const { return m_ObjName; }
        const std::string& GetTreeName() const { return m_TreeName; }
        void SetTreeName(const std::string& value) { m_TreeName = value; }
        void SetObjName(const std:string& value) { m_ObjName = value; }

    private:
        std::string m_TreeName;  // Tree name(path) of this tree
        std::string m_ObjName;   // Name of object holded by this tree

    ClassDef(TreeMetaData,1)

};

}

#endif
