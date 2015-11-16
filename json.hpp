#ifndef json_hpp_
#define json_hpp_

#include <string>

namespace json
{

   std::string
   escape (const std::string & s)
   {
      std::string es;
      es.reserve (2 * s.size ());

      for (const auto & e : s)
      {
         if (e == '"')
         {
            es.push_back ('\\');
            es.push_back ('"');
         }
         else
         {
            es.push_back (e);
         }
      }

      return es;
   }

}

#endif
