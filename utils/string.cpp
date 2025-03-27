#include "string.h"
#include <ostream>

namespace utils {
   std::ostream &operator<<( std::ostream &os, const string &s ){
      os << s.cstr();
      return os;
   }
}
