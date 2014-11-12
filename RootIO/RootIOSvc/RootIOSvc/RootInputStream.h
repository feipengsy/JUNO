#ifndef ROOTINPUTSTREAM_H
#define ROOTINPUTSTREAM_H

#include "RootIOSvc/BaseIOStream.h"
#include <vector>
#include <string>

class NavTreeList;
class DataRegistritionSvc;

namespace JM {
    class EvtNavigator;
}

class RootInputStream : public BaseIOStream {

public:

    RootInputStream(DataRegistritionSvc* regSvc);
    ~RootInputStream();

    bool init();

    /// Get current index of NavTreeList
    int treeIndex();

    /// Get current entry number
    int getEntry() { return m_entry; }

    /// Get NavTreeList
    NavTreeList* getTrees() { return m_trees; }

    void registerTreeList(NavTreeList* ntl);

    void registerNavPaths(const std::vector<std::string>& paths, const std::vector<std::string>& eventNames);

    /// Read in current entry
    bool read();

    /// Get the EvtNavigator just read
    JM::EvtNavigator* get();

    /// Set the absolute entry to read.  If read is true read() the
    /// entry.
    bool setEntry(int entry, bool read=true);

    /// Set entry to entry + steps.  If read is true read the
    /// resulting entry. 
    bool next(int steps=1, bool read=true);

    /// Set entry to entry - steps.  If read is true read the
    /// resulting entry.
    bool prev(int nsteps=1, bool read=true);

    /// Go to the very first entry
    bool first(bool read=true);

    /// Go to the very last entry
    bool last(bool read=true);

    /// Return total number of entries
    int entries();

private:
    NavTreeList* m_trees; // tree list of navigator
    std::vector<std::string> m_paths, m_eventNames;
    int m_entry;         // global entry count
    int m_entries;       // total entries;
    void* m_addr;
    bool m_initialized;
    DataRegistritionSvc* m_regSvc;
};

#endif
