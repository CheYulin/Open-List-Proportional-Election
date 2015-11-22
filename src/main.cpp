
#include "open_list_input_output.h"


int main() {
    string in_file_name = "/home/cheyulin/gitrepos/Open-List-Proportional-Representation/input/input_example1_seatnum_2_group1_3_group2_2.txt";
    IOProcessor my_io_processor;
    Solver *my_solver = my_io_processor.GetSolver(in_file_name);
    my_solver->getFirst_party_()->InitStrategies();
    my_solver->getSecond_party_()->InitStrategies();
    my_solver->PrintNashEquilibrium();
    delete my_solver;
    return 0;
}