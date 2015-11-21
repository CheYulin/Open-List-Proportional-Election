//
// Created by cheyulin on 11/21/15.
//

#include "test_unit.h"

void TestUnit::TestCandidatesInfo(Party *party) {
    map<CandidateId, CandidateInfo> candidates_info = party->getCandidates_info_();
    for (auto candidate_info : candidates_info) {
        cout << candidate_info.first << "," << candidate_info.second.candidate_name_ << "," <<
        candidate_info.second.candidate_vote_count_ << endl;
    }
    cout << endl;
}

void TestUnit::TestPartitionDetail(Party *party) {
    TestCandidatesInfo(party);
    for (CompareCandidatesGroupSet sets: party->getGroups_info_with_different_size_()) {
        for (Group data: sets) {
            cout << "(";
            for (int integer : data.candidates_) {
                cout << integer;
            }
            cout << "," << data.group_vote_count_ << ")" << "  ";
        }
        cout << endl;
    }
}

void TestUnit::TestGroupsInit(Solver *solver) {
    TestPartitionDetail(solver->getFirst_party_());
    cout << endl;
    TestPartitionDetail(solver->getSecond_party_());
}

void TestUnit::TestFindCertainGroup(const Group &to_be_find_group, Party *party) {
    const Group *group = party->GetExactGroupPointer(to_be_find_group);
    cout << endl;
    cout << group->GetCandidatesAsString();
    cout << endl;
    cout << group->group_vote_count_;
}

void TestUnit::TestSeatNum(Solver *solver) {
    cout << "seat nums:" << solver->getSeats_num_() << endl;
    cout << endl;
}

void TestUnit::TestStrategiesInit(Party *party) {
    int count =0;
   cout << endl;
    party->InitStrategies();
    vector<SameSizeStrategies> different_size_strategies = party->getStrategies_with_different_size_();
    for (SameSizeStrategies same_size_strategies : different_size_strategies) {
        for (Strategy strategy: same_size_strategies) {
            CompareVoteGroupPriorityQueue compare_vote_group_priority_queue = strategy.groups_info_;
            stringstream string_builder;
            while (!compare_vote_group_priority_queue.empty()) {
                const Group *group = compare_vote_group_priority_queue.top();
                string_builder << group->GetCandidatesAsString() << "," ;
                compare_vote_group_priority_queue.pop();
            }
            count++;
            cout << string_builder.str() << "    ";
        }
        cout << endl << endl;
        cout << count << endl;
    }
}
