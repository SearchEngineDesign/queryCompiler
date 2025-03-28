#include "string.h"
#include <ostream>

std::ostream &operator<<( std::ostream &os, const string &s ){
   os << s.cstr();
   return os;
}