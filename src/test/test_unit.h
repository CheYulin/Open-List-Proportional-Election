//
// Created by cheyulin on 11/21/15.
//

#ifndef SRC_TEST_UNIT_H
#define SRC_TEST_UNIT_H

#endif //SRC_TEST_UNIT_H

#include "../open_list_input_output.h"

class TestUnit{
private:
    void TestCandidatesInfo(Party *party);
    void TestPartitionDetail(Party *party);

public:
    void TestGroupsInit(Solver *solver);
    void TestFindCertainGroup(const Group& to_be_find_group, Party* party);
    void TestSeatNum(Solver *solver);
    void TestStrategiesInit(Party *party);
};