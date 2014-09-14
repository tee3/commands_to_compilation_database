#ifndef string_algorithms_hpp_
#define string_algorithms_hpp_

#include <string>

namespace string_algorithm
{

   std::string
   trim (const std::string & s)
   {
      auto s_begin =
         std::find_if (s.begin (),
                       s.end (),
                       [](const char c) { return !std::isspace (c); });
      if (s_begin == s.end ())
      {
         return std::string ();
      }

      auto s_end =
         std::find_if (s.rbegin (),
                       s.rend (),
                       [](const char c) { return !std::isspace (c) ; });

      return std::string (s_begin,s_end.base ());
   }

}

#endif
