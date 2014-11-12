#include "RootIOUtil/SmartRef.h"
#include "RootIOUtil/InputElementKeeper.h"
#include "TProcessID.h"
#include "TBranch.h"

ClassImp(JM::SmartRef);

JM::SmartRef::~SmartRef() {
  // Destructor
  clear();
}

JM::SmartRef::SmartRef(const JM::SmartRef& smartref) 
    : TObject(smartref), m_entry(smartref.m_entry), m_ref(smartref.m_ref), m_IsReferring(smartref.m_IsReferring) {
  // Copy constructor
  
  if (m_IsReferring) {
    // need to add reference count
    JM::EventObject* obj = (JM::EventObject*)GetObject();
    obj->AddRef();
  }
}

JM::SmartRef& JM::SmartRef::operator=(const JM::SmartRef& smartref)
{
  // Assign operator

  TObject::operator=(smartref);
  m_ref = smartref.m_ref;
  m_entry = smartref.entry();
  m_IsReferring = smartref.m_IsReferring;

  if (m_IsReferring) {
    // need to add reference count
    JM::EventObject* obj = (JM::EventObject*)GetObject();
    obj->AddRef();
  }

  return *this;
}

void JM::SmartRef::clear() {
  // Clear the referenced object

  if (!m_IsReferring) {
    m_ref = TRef();
    m_entry = -1;
    return;
  } 
  // Substract the reference count 
  JM::EventObject* obj = (JM::EventObject*)GetObject();

  //FIXME Sometimes TProcessID will be missing, can't find the object(during destructing of the whole system)
  if (obj) {
    int rc = obj->DesRef();
    if (0 == rc) {
      delete obj;
      InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper(false);
      Int_t uid = m_ref.GetUniqueID();
      TProcessID* pid = m_ref.GetPID();
      if (keeper) {
          keeper->DelObj(uid, pid, m_entry);
      }
    }
  }
  m_ref = TRef();
  m_entry = -1;
  m_IsReferring = false;
}


void JM::SmartRef::operator=(TObject* obj)
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

void JM::SmartRef::SetObject(TObject* obj)
{
  // Set the reference object
  
  // if the object is the same as orig, do nothing
  if (m_IsReferring and m_ref.GetObject() == obj) {
    return;
  }
  
  // SmartRef may be already referring to a object
  if (m_IsReferring) {
    // Clear first
    clear();
  }
  m_ref = obj;
  ((JM::EventObject*)obj)->AddRef();
  m_IsReferring = true;
}

JM::EventObject* JM::SmartRef::GetObject()
{
  // Return a pointer to the referenced object.
  
  UInt_t uid = m_ref.GetUniqueID();
  TProcessID* pid = m_ref.GetPID();
  if (!pid) return 0;
  if (!TProcessID::IsValid(pid)) return 0;

  // The referenced object may have already in memory
  JM::EventObject *obj = (JM::EventObject*)pid->GetObjectWithID(uid);
  if (obj) {
    // The referenced object may not be loaded by this SmartRef
    if (!m_IsReferring) {
      // Add reference count
      obj->AddRef();
      m_IsReferring = true;
    }
  return obj;
  }

  // The referenced object may be in the SmartRefTable
  InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper();
  TBranch* branch =  keeper->GetBranch(uid, pid);
  if (!branch) return 0;
  void* addr = 0;
  branch->SetAddress(&addr);
  branch->GetEntry(m_entry);

  obj = (JM::EventObject*)addr;
  // Add reference count
  obj->AddRef();
  m_IsReferring = true;
  return obj;
}

void JM::SmartRef::Streamer(TBuffer &R__b)
{
   // Stream an object of class JM::SmartRef.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(JM::SmartRef::Class(),this);
      InputElementKeeper* keeper = InputElementKeeper::GetInputElementKeeper(false);
      Int_t uid = m_ref.GetUniqueID();
      TProcessID* pid = m_ref.GetPID();
      keeper->AddObjRef(uid, pid);
   } else {
      R__b.WriteClassBuffer(JM::SmartRef::Class(),this);
   }
}
