#include "DataRegistritionSvc/BookEDM.h"
#include "DataRegistritionSvc/EDMManager.h"

BookEDM::BookEDM(const std::string& headerName, const std::string& eventName, int priority, const std::string& defaultPath)
{
    EDMManager::get()->book(headerName, eventName, priority, defaultPath);
}
