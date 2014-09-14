#ifndef filesystem_hpp_
#define filesystem_hpp_

#include <fstream>

#include <algorithm>

#include <string>

namespace filesystem
{

   std::string
   normalize_path (const std::string & s)
   {
      /// @todo implement this
      return s;
   }

   bool
   is_absolute (const std::string & s)
   {
      return s [0] == '/';
   }

   std::string
   to_absolute (const std::string & s)
   {
      /// @todo do this, use filesystem spec
      return s;
   }

   std::string
   extension (const std::string & s)
   {
      auto s_i = std::find (s.rbegin (),s.rend (),'.');
      ++s_i;

      return std::string (s_i.base (),s.end ());
   }

   bool
   exists (const std::string & s)
   {
      try
      {
         /// @todo pretty heavy way to do this
         std::ifstream ifs (s);

         return true;
      }
      catch (...)
      {
      }

      return false;
   }

}

#endif
