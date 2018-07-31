# Open List Proportional Representation
## Implementations and Build:

1. Codes for testing are put in **/src/test**  
  Some of them are deprecated.  

2. Main functional codes are put in **src/**  
    - **open_list_includes.h** : includes namespace std and all used stl headers  
    - **open_list_party.h, open_list_party.cpp** : includes declaration and definition of structs and classes for parties  
    - **open_list_solver.h open_list_solver.cpp** : includes declaration and definition of structs and classes for solver for this problem  
    - **open_list_input_ouput.h open_list_input_output.cpp** : includes input file handling and solver creation  
    - **main.cpp** : entrance of the program  

3. Build Information  
    - use Cmake  
    	in src directory, you can find CMakeLists.txt
    - use gnu make
    	in src directory, you can find Makefile
	I have tested with **gcc 4.8, gnu make 3.8, cmake 3.3**.  

## Notes:
1. This project provides three types of solvers for the open list proportional representation.  
	- One is **navive one**, which travserses whole searching space once.  
	- Second is **alpha-beta pruning solver in brach master**, which traverses two rounds, but prunes impossible searching guys. 
	- Third is also **alpha-beta pruning solver in branch dev-alphabeta-twobitmaps**. The difference between second and third lays in storage for nash equilibriums and searching strategies. The second begin to output after fisrt round, during the checking period in second round while the third begins to output after geting the whole result space.  

2. This project can deal with 9 by 9 case easily. And for 10 by 10 cases, it can begin to output within five minutes on lab machine.  
