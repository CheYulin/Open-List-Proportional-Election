#include "open_list_input_output.h"


int main(int argc, char* argv[]) {
    string in_file_name = argv[1];
    IOProcessor my_io_processor;
    Solver *my_solver = my_io_processor.GetSolver(in_file_name);
    my_solver->getFirst_party_()->InitStrategies();
    my_solver->getSecond_party_()->InitStrategies();
    Party *first_party = new Party(*my_solver->getFirst_party_());
    Party *second_party = new Party(*my_solver->getSecond_party_());
    Solver *alpha_beta_solver = new AlphaBetaPruningSolverWithBits(first_party,second_party,my_solver->getSeats_num_());
    alpha_beta_solver->PrintNashEquilibrium();
//    my_solver->PrintNashEquilibrium();
    delete my_solver;
    return 0;
}