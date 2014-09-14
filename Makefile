CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++

HEADERS = filesystem.hpp json.hpp string_algorithm.hpp

commands_to_compilation_database_cpp.cpp: $(HEADERS)

commands_to_compilation_database_cpp: commands_to_compilation_database_cpp.cpp

all: commands_to_compilation_database_cpp

test: all
	./commands_to_compilation_database_py --build-tool=Boost.Build --output-filename compile_commands_py.json < commands_to_compilation_database_Boost.Build.txt
	diff -w commands_to_compilation_database_Boost.Build.json compile_commands_py.json
	./commands_to_compilation_database_cpp < commands_to_compilation_database_Boost.Build.txt
	diff -w commands_to_compilation_database_Boost.Build.json compile_commands.json
