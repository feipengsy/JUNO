#include "DataRegistritionSvc/BookEDM.h"
#include "DataRegistritionSvc/EDMRegistritionTable.h"

BookEDM::BookEDM(const std::string& headerName, const std::string& eventName, int priority, const std::string& path)
{
    EDMRegistritionTable::get()->book(headerName, eventName, priority, path);
}
