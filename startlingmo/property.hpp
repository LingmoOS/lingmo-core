#ifndef PROPERTY_HPP
#define PROPERTY_HPP

#include <cassert>

#define READ_ONLY 1
#define WRITE_ONLY 2
#define READ_WRITE 3

template<typename Container, typename ValueType, int nPropType>
class LProperty {
 public:
  LProperty();
  
  void setContainer(Container *cObject);
  void setter(void (Container::*pSet)(ValueType value));
  void getter(ValueType (Container::*pGet)());
  
  ValueType operator=(const ValueType &value);
  operator ValueType();
  
 private:
  Container *m_cObject;
  void (Container::*Set)(ValueType value);
  ValueType (Container::*Get)();
};

#endif // PROPERTY_HPP