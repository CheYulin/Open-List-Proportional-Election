#include "open_list_input_output.h"


int main(int argc, char* argv[]) {
    string in_file_name = argv[1];

    IOProcessor my_io_processor;
    SolverInfo solver_info = my_io_processor.GetSolverInfo(in_file_name);
    Party *first_party = solver_info.first_party_;
    Party *second_party = solver_info.second_party_;
    Solver *alpha_beta_solver = new AlphaBetaPruningSolverWithBits(first_party,second_party,solver_info.seat_num);
    alpha_beta_solver->PrintNashEquilibrium();
    delete alpha_beta_solver;
    return 0;
}