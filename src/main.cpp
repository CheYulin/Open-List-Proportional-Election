
#include "open_list_input_output.h"


int main() {
    string in_file_name = "/home/cheyulin/gitrepos/Open-List-Proportional-Representation/input/input_example7_seatnum_2_group1_5_group2_4.txt";
    IOProcessor my_io_processor;
    Solver *my_solver = my_io_processor.GetSolver(in_file_name);

    delete my_solver;
    return 0;
}