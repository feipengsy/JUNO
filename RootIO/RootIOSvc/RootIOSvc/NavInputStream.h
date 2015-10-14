#ifndef NAV_INPUT_STREAM
#define NAV_INPUT_STREAM 0

#include "IInputStream.h"
#include "EvtNavigator/EvtNavigator.h"

#include <stirng>
#include <vector>

class NavInputStream : public IInputStream {

    typedef std::vector<std::string> StringVector;

    public:
        NavInputStream(const StringVector& file);
        ~NavInputStream();
        /// iniaizlie NavInputStream.
        bool initialize();
        /// Get the EvtNavigator just read.
        JM::EvtNavigator* get();
        /// Set the absolute entry to read.  If read is true read entry
        bool setEntry(int entry, bool read=true);
        /// Set entry to entry + steps.  If read is true read the resulting entry
        bool next(int steps=1, bool read=true);
        /// Set entry to entry - steps.  If read is true read the resluting entry
        bool prev(int nsteps=1, bool read=true);
        /// Go to the very first entry.
        bool first(bool read=true);
        /// Go to the very last entry.
        bool last(bool read=true);

    private:
        /// Read in current entry.
        bool read();

    private:
        NavTreeList*    m_trees;   // Implementation of TTrees list holding EvtNavigators
        long            m_entry;   // Current entry number
        StringVector    m_files;   // File list
        StringVector    m_navPath; // Event path list of EvtNavigators
};


#endif
