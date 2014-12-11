#include "RootIOUtil/SmartRef.h"
#include "RootIOUtil/InputElementKeeper.h"
#include "Event/EventObject.h"
#include "TClass.h"
#include "TROOT.h"
#include "TProcessID.h"
#include "TBranch.h"

ClassImp(JM::SmartRef);

JM::SmartRef::SmartRef() 
    : m_entry(-1)
    , m_branchID(-1)
    , m_refObjTemp(0)
    , m_pid(0) 
{
}

JM::SmartRef::SmartRef(const JM::SmartRef& ref) 
    : TObject(ref)
{
  // Copy constructor
  *this = ref;
}

JM::SmartRef::~SmartRef() {
  // Destructor
  clear();
}

JM::SmartRef& JM::SmartRef::operator=(const JM::SmartRef& ref)
{
  // Assign operator
  
  if (this != &ref) {
    SetUniqueID(ref.GetUniqueID());
    m_entry = ref.m_entry;
    m_branchID = ref.m_branchID;
    m_refObjTemp = ref.m_refObjTemp;
    m_pid = ref.m_pid;
    SetBit(kHasUUID, ref.TestBit(kHasUUID));

    if (m_refObjTemp) {
      // need to add reference count
      m_refObjTemp->AddRef();
    }
  }

  return *this;
}

bool JM::operator==(const JM::SmartRef& r1, const JM::SmartRef& r2)
{
  // Return true if r1 and r2 point to the same object.

  if (r1.GetPID() == r2.GetPID() && r1.GetUniqueID() == r2.GetUniqueID()) return true;
  return false;
}

bool JM::operator!=(const JM::SmartRef& r1, const JM::SmartRef& r2)
{
  // Return true if r1 and r2 do not point to the same object.
  return !(r1 == r2);
}

void JM::SmartRef::clear() {
  // Clear the referenced object

  if (!m_refObjTemp) {
    m_pid = 0;
    m_entry = -1;
    return;
  } 

  // Substract the reference count 
  int rc = m_refObjTemp->DesRef();
  if (0 == rc) {
    delete m_refObjTemp;
    InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper(false);
    if (keeper && TProcessID::IsValid(m_pid)) {
        UInt_t uid = GetUniqueID();
        keeper->DelObj(uid, m_pid, m_entry);
    }
  }
  m_pid = 0;
  m_entry = -1;
  m_branchID = -1;
  m_refObjTemp = 0;
}

void JM::SmartRef::operator=(JM::EventObject* obj)
{
  // Set the reference object

  SetObject(obj);
}

JM::EventObject* JM::SmartRef::operator->() {
  // Get the referenced object

  return GetObject();
}

JM::EventObject& JM::SmartRef::operator*() {
  // Get the referenced object
  
  return *GetObject();
}

void JM::SmartRef::SetObject(JM::EventObject* obj)
{
  // Set the reference object

  if (!obj) return; 

  // if the object is the same as orig, do nothing
  if (m_refObjTemp == obj) return;
  
  // SmartRef may be already referring to a object
  if (m_refObjTemp) {
    // Clear first
    clear();
  }

  UInt_t uid = 0;
  
  // TObject Streamer must not be ignored.
  if (obj->IsA()->CanIgnoreTObjectStreamer()) return;

  if (obj->TestBit(kIsReferenced)) {
    uid = obj->GetUniqueID();
  } 
  else {
    uid = TProcessID::AssignID(obj);
  }
  m_pid = TProcessID::GetProcessWithUID(obj->GetUniqueID(),obj);
  ResetBit(kHasUUID);
  SetUniqueID(uid); 

  obj->AddRef();
  m_refObjTemp = obj;
}

void JM::SmartRef::SetBranchID(Short_t value)
{
  m_branchID = value;
}

JM::EventObject* JM::SmartRef::GetObject()
{
  // Return a pointer to the referenced object.

  // Referenced object has been already set or loaded
  if (m_refObjTemp) return m_refObjTemp;

  UInt_t uid = GetUniqueID();
  
  // Check if m_pid is valid
  if (!m_pid) return 0;
  if (!TProcessID::IsValid(m_pid)) return 0;

  // Search the referenced object in the memory
  JM::EventObject *obj = (JM::EventObject*)m_pid->GetObjectWithID(uid);
  if (obj) {
    // The referenced object was not loaded by this SmartRef, add reference count
    obj->AddRef();
    m_refObjTemp = obj;
    return obj;
  }

  // Search the referenced object in SmartRefTable
  InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper();
  TBranch* branch =  keeper->GetBranch(uid, m_pid, m_branchID);
  if (!branch) return 0;
  // Load the referenced object
  void* addr = 0;
  branch->SetAddress(&addr);
  branch->GetEntry(m_entry);

  obj = (JM::EventObject*)addr;
  // Add reference count
  obj->AddRef();
  m_refObjTemp = obj;
  return obj;
}

void JM::SmartRef::Streamer(TBuffer &R__b)
{
   // Stream an object of class JM::SmartRef.

  UShort_t pidf;
  if (R__b.IsReading()) {
    // Reading
    TObject::Streamer(R__b);
    R__b >> pidf;
    R__b >> m_entry;
    pidf += R__b.GetPidOffset();
    m_pid = R__b.ReadProcessID(pidf);
    InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper(false);
    Int_t uid = GetUniqueID();
    if (keeper) keeper->AddObjRef(uid, m_pid);
  } 
  else {
    // Writing
    TObject::Streamer(R__b);
    pidf = R__b.WriteProcessID(m_pid);
    R__b << pidf;
    R__b << m_entry;
   }
}
