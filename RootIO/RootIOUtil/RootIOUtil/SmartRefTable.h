/*  Class SmartRefTable
 *
 *  SmartRefTable holds the tree id and branch id of all 
 *  object referenced by SmartRef. During intializing
 *  of RootInputStream, information of referenced object
 *  will be added into this table.
 *
 *  When a SmartRef tries to load the object it refers
 *  to, it will query SmartRefTable to get the tree id
 *  and file id allocated by InputTreeManager and 
 *  InputFileManager.
 *  
 *  This class is inspired by TRefTable in ROOT. 
 */

#ifndef ROOTIOUTIL_SMARTREFTABLE_H
#define ROOTIOUTIL_SMARTREFTABLE_H

#include "TObject.h"

#include <vector>
#include <string>
#include <list>

class TProcessID;
class SmartRefTableImpl;

namespace JM {

    class TablePerTree;

}

class SmartRefTable {
 //Table of referenced objects during an I/O operation

    typedef std::list<SmartRefTableImpl*>::iterator SRTIterator;

public:
    SmartRefTable();
    ~SmartRefTable();
    // Given a SmartRef, get the id of the tree holding the object if refers to
    Int_t GetTreeID(Int_t uid, const TProcessID* pid);
    // Given a SmartRef, get the id of the branch holding the referenced object
    Int_t GetBranchID(Int_t uid, const TProcessID* pid);
    // Static method to get the current SmartRefTable
    static SmartRefTable* GetRefTable();
    // Read UniqueIDTable, register all referenced object it holds
    void ReadMetaData(JM::TablePerTree* table, Int_t treeid);
    // Start a new table for a new input file
    void StartNewTable(Int_t fileid);
    // Delete the table for a given file
    void DeleteTable(Int_t fileid);
    
private:
    std::list<SmartRefTableImpl*>  m_tableList;  // List of the SmartRefTableImpl*(one for each file) 
    static SmartRefTable*          fgSmartRefTable; // Current SmartRefTable
};

#endif
