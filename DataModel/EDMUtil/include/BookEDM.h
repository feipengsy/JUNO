#ifndef BOOK_EDM_H
#define BOOK_EDM_H

#include <string>

struct BookEDM {
    
    BookEDM(const std::string& headerName, const std::string& eventNames, int priority, const std::string& path);

};

#define JUNO_BOOK_EDM(headerName, eventNames, defaultPriority, defaultPath) \
BookEDM BookEDM_##defaultPriority##_(#headerName, #eventNames, defaultPriority, #defaultPath)

#endif
