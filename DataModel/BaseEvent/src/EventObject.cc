#include "Event/EventObject.h"
ClassImp(JM::EventObject);

unsigned int JM::EventObject::AddRef() 
{
  return ++m_RefCount;
}

unsigned int JM::EventObject::DesRef() 
{
  if (m_RefCount > 0) return --m_RefCount;
  return 0;
}
