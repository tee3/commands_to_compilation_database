all:

test: all
	./commands_to_compilation_database_py --build-tool=Boost.Build --output-filename compile_commands_py.json < commands_to_compilation_database_Boost.Build.txt
	diff -w commands_to_compilation_database_Boost.Build.json compile_commands_py.json
