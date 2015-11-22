//
// Created by cheyulin on 11/21/15.
//

#ifndef SRC_TEST_UNIT_H
#define SRC_TEST_UNIT_H

#endif //SRC_TEST_UNIT_H

#include "../open_list_input_output.h"

class TestUnit {
private:
    void TestCandidatesInfo(Party *party);

    void TestPartitionDetail(Party *party);

public:

    void TestGroupsInit(Solver *solver);

    void TestFindCertainGroup(const Group &to_be_find_group, Party *party);

    void TestSeatNum(Solver *solver);

    void TestStrategiesInit(Party *party);

    void TestPayoffCompute(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party, Solver *solver);

    void TestNashEquilibriumWithBruteForceSolver(Solver *parent_solver);
};

class BruteForceSolver : public Solver {

private:
    void TraverseStrategyFromPerspectiveOfOneParty(vector<SameSizeStrategies> *store_different_size_strategies,
                                                   vector<SameSizeStrategies> *fixed_different_size_strategies);

    void TraverseProfileToCheckNashEquilibrium(vector<SameSizeStrategies> *store_different_size_strategies,
                                               vector<SameSizeStrategies> *fixed_different_size_strategies);

    virtual void FindNashEquilibrium(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party);

public:
    BruteForceSolver(Party *first_party, Party *second_party, int seats_num);

};