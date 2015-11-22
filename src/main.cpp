#include "open_list_input_output.h"


int main(int argc, char* argv[]) {
    string in_file_name = argv[1];
    IOProcessor my_io_processor;
    Solver *my_solver = my_io_processor.GetSolver(in_file_name);
    my_solver->getFirst_party_()->InitStrategies();
    my_solver->getSecond_party_()->InitStrategies();
    my_solver->PrintNashEquilibrium();
    delete my_solver;
    return 0;
}