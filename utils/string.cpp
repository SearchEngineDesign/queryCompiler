#include "string.h"
#include <ostream>

std::ostream &operator<<( std::ostream &os, const string &s ){
   for (int i = 0; i < s.size(); i++)
      os << s[i];
   return os;
}