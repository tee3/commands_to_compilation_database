#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <boost/algorithm/string.hpp>

#include <map>
#include <vector>
#include <string>

#include <regex>
#include "compilation_database.hpp"

struct arguments_type
{
   bool help;
   boost::filesystem::path root_directory;
   std::vector<boost::filesystem::path> compilers;
   std::vector<boost::filesystem::path> extensions;
   std::string build_tool;
   std::string compile_command_regex;
   bool incremental;
   boost::filesystem::path output_filename;
};

int
main (int argc, char * argv [])
{
   auto description = "Generate a Clang compilation database from compiler commands.";

   std::vector<boost::filesystem::path> default_compilers =
   {
      "cc",
      "c++",

      "clang",
      "clang++",

      "gcc",
      "g++",

      "cl",

      // for now
      "/usr/local/bin/clang-3.4",
      "/usr/local/bin/clang++-3.4"
   };

   std::vector<boost::filesystem::path> c_extensions =
   {
      ".c",
      ".h"
   };

   std::vector<boost::filesystem::path> cxx_extensions =
   {
      ".cpp",
      ".hpp",
      ".cc",
      ".hh",
      ".cxx",
      ".hxx",
      ".C",
      ".H"
   };

   std::vector<boost::filesystem::path> objc_extensions =
   {
      ".m"
   };

   std::vector<boost::filesystem::path> objcxx_extensions =
   {
      ".mm"
   };

   std::vector<boost::filesystem::path> default_extensions;
   default_extensions.insert (default_extensions.end (),
                              c_extensions.begin (),
                              c_extensions.end ());
   default_extensions.insert (default_extensions.end (),
                              cxx_extensions.begin (),
                              cxx_extensions.end ());
   default_extensions.insert (default_extensions.end (),
                              objc_extensions.begin (),
                              objc_extensions.end ());
   default_extensions.insert (default_extensions.end (),
                              objcxx_extensions.begin (),
                              objcxx_extensions.end ());

   arguments_type args;

   boost::program_options::options_description parser (description);
   parser.add_options ()
      (
         "help,h",
	 boost::program_options::bool_switch (&args.help)->default_value (false),
         "Print the help."
      )
      (
         "root-directory",
         boost::program_options::value<boost::filesystem::path> (&args.root_directory)->default_value (""),
         "The root directory for the project."
      )
      (
         "compilers",
         boost::program_options::value<std::vector<boost::filesystem::path>> (&args.compilers)->default_value (default_compilers,""),
         "A list of additional compilers."
      )
      (
         "extensions",
         boost::program_options::value<std::vector<boost::filesystem::path>> (&args.extensions)->composing ()->default_value (default_extensions,""),
         "A list of additional filename extensions."
      )
      (
         "build-tool",
         boost::program_options::value<std::string> (&args.build_tool)->default_value (""),
         "The build tool that generated the input."
      )
      (
         "compile-command-regex",
         boost::program_options::value<std::string> (&args.compile_command_regex)->default_value (""),
         "A regular expression to parse the command line into the compiler, flags, and filename."
      )
      (
         "incremental",
         boost::program_options::bool_switch (&args.incremental)->default_value (false),
         "Incrementally update existing database."
      )
      (
         "output-filename,o",
         boost::program_options::value<boost::filesystem::path> (&args.output_filename)->default_value ("compile_commands.json"),
         "The filename of the compilation database."
      )
      ;

   boost::program_options::variables_map vm;
   boost::program_options::store (boost::program_options::parse_command_line (argc,
                                                                              argv,
                                                                              parser),
                                  vm);
   boost::program_options::notify (vm);

   if (args.help)
   {
      std::cout << parser << "\n";
      return 1;
   }

   args.output_filename =
      boost::filesystem::absolute (args.output_filename);

   std::regex compile_command_regex;
   if (args.compile_command_regex == "")
   {
      if ((args.build_tool == "") ||
          (args.build_tool == "make"))
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

   // create the initial compilation database
   compilation_database::compilation_database_type compilation_database;
   if (args.incremental)
   {
      if (boost::filesystem::exists (args.output_filename))
      {
         std::ifstream ifs (args.output_filename.c_str ());

         compilation_database = compilation_database::load (ifs);
      }
   }

   // generate a more efficient compilation map by filename
   std::map<std::string,compilation_database::compilation_database_entry> compilation_map;
   for (const auto & entry : compilation_map)
   {
      boost::filesystem::path f (entry.second.filename);
      if (!f.is_absolute ())
      {
         f = entry.second.directory / f;
      }
      compilation_map [f.string ()] = entry.second;
   }

   // parse the compilation log and update the compilation database
   std::string line;
   std::smatch m;

   while (std::cin)
   {
      std::getline (std::cin,line);
      boost::trim (line);

      if (line == "")
      {
         continue;
      }

      std::regex_match (line,m,compile_command_regex);

      if (!m.ready ())
      {
         continue;
      }

      if (m.size () != 4)
      {
         continue;
      }

      const boost::filesystem::path compiler (m [1]);
#if 0
      const std::string flags = m [2];
#endif
      boost::filesystem::path filename (m [3]);

      // check if the filename extension is supported
      auto n = filename.parent_path () / filename.stem ();
      auto e = filename.extension ();

      if (std::find (std::begin (args.compilers),
                     std::end (args.compilers),
                     compiler.string ()) ==
          std::end (args.compilers))
      {
         continue;
      }

      if (std::find (std::begin (args.extensions),
                     std::end (args.extensions),
                     e) ==
          std::end (args.extensions))
      {
         continue;
      }

      const compilation_database::compilation_database_entry entry =
      {
         args.root_directory != "" ? args.root_directory : boost::filesystem::current_path (),
         line,
         filename
      };

      auto f = entry.filename;
      if (!f.is_absolute ())
      {
         f = entry.directory / f;
      }

      compilation_map [f.string ()] = entry;
   }

   compilation_database.clear ();
   for (const auto & entry : compilation_map)
   {
      compilation_database.push_back(entry.second);
   }

   // print as json
   {
      std::ofstream ofs (args.output_filename.c_str ());
      compilation_database::dump (compilation_database,ofs);
   }

   return 0;
}
