#ifndef ROOT_INPUT_STREAM_H
#define ROOT_INPUT_STREAM_H 1

#include "RootIOSvc/IInputStream.h"
#include "RootIOSvc/RootIOStream.h"
#include <vector>
#include <string>

class NavTreeList;

namespace JM {
    class EvtNavigator;
}

class RootInputStream : public IInputStream, public RootIOStream {

    public:

        // RootInputStream is created by RootInputSvc
        RootInputStream(NavTreeList* ntl, const std::vector<std::string>& navPath);
        ~RootInputStream();

        // Get current index of NavTreeList
        int treeIndex();
        // Get current entry number
        int getEntry() { return m_entry; }
        // Get the EvtNavigator just read
        JM::EvtNavigator* get();
        // Set the absolute entry to read.  If read is true read the resulting entry.
        bool setEntry(int entry, bool read=true);
        // Set entry to entry + steps.  If read is true read the resulting entry. 
        bool next(int steps=1, bool read=true);
        // Set entry to entry - steps.  If read is true read the resulting entry.
        bool prev(int nsteps=1, bool read=true);
        // Go to the very first entry
        bool first(bool read=true);
        // Go to the very last entry
        bool last(bool read=true);
        // Return total number of entries
        int entries();

    private:
        // Read in Current entry
        bool read();

        NavTreeList* m_trees;               // Tree list of EvtNavigator
        std::vector<std::string> m_paths;   // Path vector saved in EvtNavigator
        int m_entry;                        // Global entry count
        int m_entries;                      // Total entries;
        bool m_initialized;
};

#endif
