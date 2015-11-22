#Open List Proportional Representation
1. Codes for testing are put in /src/test
   1.1 test_unit.h test_unit.cpp test_main 
    1)test for seat number, group initialization for two parties, 
    2)test for strategy initialization for two praties
    3)test for profile/payoff traverse in all combination of two parties strategies. 
    4)test of brute force solver to check the correctness of implemented solver.
   1.2 depracated/* includes deprecated testing files

2. Main functional codes are put in src/ 
	2.1 open_list_includes.h 
	    includes namespace std and all used stl headers
	2.2 open_list_party.h open_list_party.cpp 
	    includes declaration and definition of structs and classes for parties
	2.3 open_list_solver.h open_list_solver.cpp 
	    includes declaration and definition of structs and classes for solver for this problem
	2.4 open_list_input_ouput.h open_list_input_output.cpp 
		includes input file handling and solver creation
	2.5 main.cpp is entrance of the program

Notes:
 1)This naive algorithm traverses strategy combination of two parties once, i.e. and use O(strategy number of party) memory space. 
 2)Using brute force needs three traverses, first two to get informaiton of max payoff, and third to check nash equilibrium.
 3)Both naive and brute force all exclude/cut those combination which could not reach the given seats.

