#ifndef ROOTIOUTIL_TREEMETADATA_H
#define ROOTIOUTIL_TREEMETADATA_H

#include "TObject.h"
#include <string>

namespace JM {

class TreeMetaData : public TObject {

    public:
        TreeMetaData() {}
        ~TreeMetaData() {}

        std::string& GetTreeName() { return m_TreeName; }
        void SetTreeName(const std::string& value) { m_TreeName = value; }

    private:
        std::string m_TreeName;  // Tree name(path) of this Tree

    ClassDef(TreeMetaData,1)

};

}

#endif
