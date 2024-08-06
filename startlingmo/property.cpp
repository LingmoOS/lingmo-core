#include "property.hpp"

template<typename Container, typename ValueType, int nPropType>
LProperty<Container, ValueType, nPropType>::LProperty() :
  m_cObject(nullptr),
  Set(nullptr),
  Get(nullptr)
{}

template<typename Container, typename ValueType, int nPropType>
void LProperty<Container, ValueType, nPropType>::setContainer(Container *cObject) {
  m_cObject = cObject;
}

template<typename Container, typename ValueType, int nPropType>
void LProperty<Container, ValueType, nPropType>::setter(void (Container::*pSet)(ValueType value)) {
  if ((nPropType == WRITE_ONLY) || (nPropType == READ_WRITE))
    Set = pSet;
  else
    Set = nullptr;
}

template<typename Container, typename ValueType, int nPropType>
void LProperty<Container, ValueType, nPropType>::getter(ValueType (Container::*pGet)()) {
  if ((nPropType == READ_ONLY) || (nPropType == READ_WRITE))
    Get = pGet;
  else
    Get = nullptr;
}

template<typename Container, typename ValueType, int nPropType>
ValueType LProperty<Container, ValueType, nPropType>::operator=(const ValueType &value) {
  assert(m_cObject != nullptr);
  assert(Set != nullptr);
  (m_cObject->*Set)(value);
  return value;
}

template<typename Container, typename ValueType, int nPropType>
LProperty<Container, ValueType, nPropType>::operator ValueType() {
  assert(m_cObject != nullptr);
  assert(Get != nullptr);
  return (m_cObject->*Get)();
}