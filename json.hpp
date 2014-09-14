#ifndef json_hpp_
#define json_hpp_

#include <iostream>

#include <vector>
#include <string>

namespace json
{

   struct compilation_database_entry
   {
      std::string directory;
      std::string command;
      std::string filename;
   };

   typedef std::vector<compilation_database_entry> compilation_database_type;

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

   void
   dump (const compilation_database_type & compilation_database,
         std::ostream & os)
   {
      os << "[\n";
      for (auto i = std::begin (compilation_database);
           i != std::end (compilation_database);
           ++i)
      {
         // sorted by keys
         os <<
            "  {" << "\n" <<
            "    \"command\": \"" << escape (i->command) << "\", " << "\n" <<
            "    \"directory\": \"" << escape (i->directory) << "\", " << "\n" <<
            "    \"file\": \"" << escape (i->filename) << "\"\n"
            "  }" << ((i != (std::end (compilation_database) - 1)) ? ", " : "") << "\n";
      }
      os << "]";
   }

}

#endif
