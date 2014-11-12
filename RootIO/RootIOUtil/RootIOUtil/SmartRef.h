#ifndef ROOTIOUTIL_SMARTREF_H
#define ROOTIOUTIL_SMARTREF_H

#include "TObject.h"
#include "Event/EventObject.h"
#include "TRef.h"
#include "TClass.h"

namespace JM 
{

  class SmartRef: public TObject
  {
  private:

    Long64_t m_entry;        // entry number of the referenced header
    TRef     m_ref;          // TRef pointed to the referenced header
    bool     m_IsReferring;  //! indicate weather SmartRef is currently referring to a object
  

  protected:


  public:
  // Default Constructor
  SmartRef() : m_entry(-1),
               m_IsReferring(false) { Class()->IgnoreTObjectStreamer(); }
  
  // Copy Constructor
  SmartRef(const SmartRef&);

  // Default Destructor
  virtual ~SmartRef();
  
  // Set the referenced object
  void operator=(TObject* value);

  // Assignment operator
  SmartRef& operator=(const SmartRef&);

  // Get the referenced object
  EventObject* operator->();

  // Get the referenced object
  EventObject& operator*();

  // Clear SmartRef
  void clear();
  
  // Set the referenced object
  void SetObject(TObject* value);
  
  // Get the refernced object
  EventObject* GetObject();
  
  // Get the entry number of the referenced object
  const Long64_t& entry() const;
  
  // Set the entry number of the referenced object
  void setEntry(const Long64_t& value);
  
  // Get the inner TRef
  const TRef& ref() const;

  // Get the inner TRef(non const)
  TRef& ref();
  
  ClassDef(SmartRef,1);
  

  }; // class SmartRef

} // namespace JM;


inline const Long64_t& JM::SmartRef::entry() const 
{
  return m_entry;
}

inline void JM::SmartRef::setEntry(const Long64_t& value) 
{
  m_entry = value;
}

inline const TRef& JM::SmartRef::ref() const 
{
  return m_ref;
}

inline TRef& JM::SmartRef::ref()
{
  return m_ref;
}

#endif ///ROOTIOUTIL_SMARTREF_H
