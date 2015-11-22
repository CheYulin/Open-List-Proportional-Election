#Open List Proportional Representation
1. Codes for testing are put in /src/test
   1.1 test_unit.h test_unit.cpp test_main includes testing for seat number, group initialization for two parties, strategy initialization for two praties and profile/payoff traverse in all combination of two parties strategies.

   1.2 depracated/* includes deprecated testing files

2. Main functional codes are put in src/ 
	2.1 open_list_includes.h includes namespace std and all used stl headers
	2.2 open_list_party.h open_list_party.cpp includes declaration and definition of structs and classes for parties
	2.3 open_list_solver.h open_list_solver.cpp includes declaration and definition of structs and classes for solver for this problem
	2.4 open_list_input_ouput.h open_list_input_output.cpp includes input file handling and solver creation
	2.5 main.cpp is entrance of the program

3. This algorithm traverses strategy combination of two parties once, i.e. and use O(strategy number of party) memory space
