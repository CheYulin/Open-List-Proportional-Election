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
    int count = 0;
    cout << endl;
    party->InitStrategies();
    vector<SameSizeStrategies> different_size_strategies = party->getStrategies_with_different_size_();
    for (SameSizeStrategies same_size_strategies : different_size_strategies) {
        for (int i = 0; i < same_size_strategies.size(); i++) {
            count++;
            cout << same_size_strategies[i].ToString() << "    ";
        }
        cout << endl << endl;

    }
    cout << count << endl;
}

void TestUnit::TestPayoffCompute(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party, Solver *solver) {
    vector<SameSizeStrategies> *fixed_different_size_strategies = &fixed_for_traverse_party->getStrategies_with_different_size_();
    vector<SameSizeStrategies> *store_different_size_strategies = &store_nash_equilibrium_party->getStrategies_with_different_size_();
    for (SameSizeStrategies &fixed_same_size_strategies : *fixed_different_size_strategies) {
        for (Strategy &fixed_strategy : fixed_same_size_strategies) {
            for (SameSizeStrategies &store_same_size_strategies : *store_different_size_strategies) {
                for (Strategy &stored_strategy: store_same_size_strategies) {
                    Profile profile = solver->ComputePayOff(&fixed_strategy,&stored_strategy);
                    cout << setiosflags(ios::fixed) << setprecision(2);
                    cout << stored_strategy.ToString() << "\t" << fixed_strategy.ToString()<< "  Payoff:" << profile.store_strategy_payoff_ <<"," <<profile.fixed_strategy_payoff_<<endl;
                }
            }
        }
    }
}

