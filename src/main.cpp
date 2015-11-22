#include "open_list_input_output.h"


int main() {
    string in_file_name = "/home/cheyulin/gitrepos/Open-List-Proportional-Representation/input/input_example8_seatnum_3_group1_5_group2_4.txt";
    IOProcessor my_io_processor;
    Solver *my_solver = my_io_processor.GetSolver(in_file_name);
    my_solver->getFirst_party_()->InitStrategies();
    my_solver->getSecond_party_()->InitStrategies();
    my_solver->PrintNashEquilibrium();
    delete my_solver;
    return 0;
}