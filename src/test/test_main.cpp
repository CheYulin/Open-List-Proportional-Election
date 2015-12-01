//
// Created by cheyulin on 11/21/15.
//
#include "test_unit.h"

int main(int argc, char* argv[]) {
    string in_file_name = argv[1];
    IOProcessor my_io_processor;
    SolverInfo solver_info = my_io_processor.GetSolverInfo(in_file_name);
    Solver *my_solver = new Solver(solver_info.first_party_,solver_info.second_party_,solver_info.seat_num);

    TestUnit test_unit;
    test_unit.TestSeatNum(my_solver);
    test_unit.TestGroupsInit(my_solver);
    test_unit.TestPayoffCompute(my_solver->getFirst_party_(), my_solver->getSecond_party_(), my_solver);
    cout << endl << endl << "Next : Nash Equ" << endl << endl;

    delete my_solver;

    return 0;
}
