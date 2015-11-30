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
   std::vector<boost::filesystem::path> extensions;
   bool incremental;
   boost::filesystem::path output_filename;
   std::string flags;
   std::string cflags;
   std::string cxxflags;
   std::string objcflags;
   std::string objcxxflags;
   std::vector<std::string> includes;
   std::vector<std::string> defines;
   std::vector<std::string> undefines;
};

int
main (int argc, char * argv [])
{
   auto description = "Generate a Clang compilation database from compiler commands.";

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
         "extensions",
         boost::program_options::value<std::vector<boost::filesystem::path>> (&args.extensions)->composing ()->default_value (default_extensions,""),
         "A list of additional filename extensions."
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
      (
         "flags",
         boost::program_options::value<std::string> (&args.flags)->default_value (""),
         "The flags to pass to the Clang compiler."
      )
      (
         "cflags",
         boost::program_options::value<std::string> (&args.cflags)->default_value (""),
         "The flags to pass to the Clang C compiler."
      )
      (
         "cxxflags",
         boost::program_options::value<std::string> (&args.cxxflags)->default_value (""),
         "The flags to pass to the Clang C++ compiler."
      )
      (
         "objcflags",
         boost::program_options::value<std::string> (&args.objcflags)->default_value (""),
         "The flags to pass to the Clang Objective-C compiler."
      )
      (
         "objcxxflags",
         boost::program_options::value<std::string> (&args.objcxxflags)->default_value (""),
         "The flags to pass to the Clang Objective-C++ compiler."
      )
      (
         "include",
         boost::program_options::value<std::vector<std::string>> (&args.includes)->composing ()->default_value ({},""),
         "An include directory to pass to the Clang compiler."
      )
      (
         "define",
         boost::program_options::value<std::vector<std::string>> (&args.defines)->composing ()->default_value ({},""),
         "A define to pass to the Clang compiler."
      )
      (
         "undefine",
         boost::program_options::value<std::vector<std::string>> (&args.undefines)->composing ()->default_value ({},""),
         "A undefine to pass to the Clang compiler."
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

   while (std::cin)
   {
      std::getline (std::cin,line);
      boost::trim (line);

      if (line == "")
      {
         continue;
      }

      const boost::filesystem::path compiler ("clang++");
      std::vector<std::string> flags = { "-c" };
      boost::filesystem::path filename (line);

      // check if the filename extension is supported
      auto n = filename.parent_path () / filename.stem ();
      auto e = filename.extension ();

      if (std::find (std::begin (args.extensions),
                     std::end (args.extensions),
                     e) ==
          std::end (args.extensions))
      {
         continue;
      }

      flags.push_back ("-o " + n.native () + ".o");

      flags.push_back (args.flags);
      if (std::find (std::begin (c_extensions),
                     std::end (c_extensions),
                     e) !=
          std::end (c_extensions))
      {
         flags.push_back (args.cflags);
      }
      if (std::find (std::begin (cxx_extensions),
                     std::end (cxx_extensions),
                     e) !=
          std::end (cxx_extensions))
      {
         flags.push_back (args.cxxflags);
      }
      if (std::find (std::begin (objc_extensions),
                     std::end (objc_extensions),
                     e) !=
          std::end (objc_extensions))
      {
         flags.push_back (args.objcflags);
      }
      if (std::find (std::begin (objcxx_extensions),
                     std::end (objcxx_extensions),
                     e) !=
          std::end (objcxx_extensions))
      {
         flags.push_back (args.objcxxflags);
      }

      for (const auto & s : args.includes)
      {
         flags.push_back ("-I" + s);
      }
      for (const auto & s : args.defines)
      {
         flags.push_back ("-D" + s);
      }
      for (const auto & s : args.undefines)
      {
         flags.push_back ("-U" + s);
      }

      std::string command = compiler.native ();
      for (const auto & s : flags)
      {
         command += " " + s;
      }
      command += " " + filename.native ();

      const compilation_database::compilation_database_entry entry =
      {
         args.root_directory != "" ? args.root_directory : boost::filesystem::current_path (),
         command,
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
