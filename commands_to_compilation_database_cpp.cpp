#include <iostream>
#include <fstream>
#include "filesystem.hpp"

#include <algorithm>
#include "string_algorithm.hpp"

#include <map>
#include <vector>
#include <string>

#include <regex>
#include "json.hpp"

struct arguments_type
{
   std::string root_directory;
   std::vector<std::string> compilers;
   std::vector<std::string> extensions;
   std::string build_tool;
   std::string compile_command_regex;
   bool incremental;
   std::string output_filename;
};

arguments_type
parse_args (int argc, char * argv [])
{
   return arguments_type
   {
      ".",
      {
         "clang",
         "clang++",
         "/usr/local/bin/clang-3.4",
         "/usr/local/bin/clang++-3.4",
      },
      {
         ".c",
         ".cpp",
         ".cxx",
         ".cc",
         ".C",
      },
      "Boost.Build",
      "",
      false,
      "compile_commands.json"
   };
}

int
main (int argc, char * argv [])
{
   auto description = "Generate a Clang compilation database from compiler commands.";

   std::vector<std::string> default_compilers = {
      "clang",
      "clang++",

      // for now
      "/usr/local/bin/clang-3.4",
      "/usr/local/bin/clang++-3.4"
   };

   std::vector<std::string> default_extensions = {
      ".c",
      ".cpp",
      ".cxx",
      ".C"
   };

   auto args = parse_args (argc,argv);

#if 0
   args.output_filename = filesystem::to_absolute (args.output_filename);
#endif

   std::regex compile_command_regex;
   if (args.compile_command_regex == "")
   {
      if (args.build_tool == "")
      {
         compile_command_regex = std::regex ("^([^ ]+) (.+) ([^ ]+)$");
      }
      else if (args.build_tool == "Boost.Build")
      {
         compile_command_regex = std::regex ("^\"([^\"]+)\" (.+) \"([^\"]+)\"$");
      }
   }
   else
   {
      if (args.build_tool != "")
      {
         std::cout << "warning: regex overriding build tool option\n";
      }

      compile_command_regex = std::regex (args.compile_command_regex);
   }

   json::compilation_database_type compilation_database;
   if (args.incremental)
   {
      if (filesystem::exists (args.output_filename))
      {
         std::ifstream ifs (args.output_filename.c_str ());

/// @todo read into vector from json
#if 0
         compilation_database = json::load (ifs);
#endif
      }
   }

   std::map<std::string,json::compilation_database_entry> compilation_map;
   for (const auto & entry : compilation_map)
   {
      std::string f (entry.second.filename);
      if (!filesystem::is_absolute (f))
      {
         f = entry.second.directory + "/" + f;
      }
      compilation_map[f] = entry.second;
   }

   std::string line;
   std::smatch m;

   while (std::cin)
   {
      std::getline (std::cin,line);
      line = string_algorithm::trim (line);

      std::regex_match (line,m,compile_command_regex);

      if (!m.ready ())
      {
         continue;
      }

      if (m.size () != 4)
      {
         continue;
      }

      const std::string compiler = m [1];
#if 0
      const std::string flags = m [2];
#endif
      const std::string filename = filesystem::normalize_path (m [3]);

      if (std::find (std::begin (args.compilers),
                     std::end (args.compilers),
                     compiler) ==
          std::end (args.compilers))
      {
         continue;
      }

      if (std::find (std::begin (args.extensions),
                     std::end (args.extensions),
                     filesystem::extension (filename)) ==
          std::end (args.extensions))
      {
         std::cout << "info: not a source file " << filename << " " << filesystem::extension (filename) << "\n";

         continue;
      }

      const json::compilation_database_entry entry =
      {
         args.root_directory,
         line,
         filename
      };

      std::string f = entry.filename;
      if (!filesystem::is_absolute (f))
      {
         f = entry.directory + '/' + f;
      }

      compilation_map [f] = entry;
   }

   compilation_database.clear ();
   for (const auto & entry : compilation_map)
   {
      compilation_database.push_back(entry.second);
   }

   // print as json
   std::ofstream ofs (args.output_filename.c_str ());
   json::dump (compilation_database,ofs);

   return 0;
}
