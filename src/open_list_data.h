//
// Created by cheyulin on 11/19/15.
//

#ifndef SRC_OPEN_LIST_DATA_H
#define SRC_OPEN_LIST_DATA_H

#endif //SRC_OPEN_LIST_DATA_H

#include <memory>
#include <map>
#include <vector>

using namespace std;

namespace election {
    typedef int CandidateId;
    typedef int CandidateVoteCount;
    typedef int GroupVoteCount;
    typedef double StrategyPayOff;

    struct Group {
        vector<CandidateId> candidates;
        int group_vote_count;
    };

    struct Strategy {
        multimap<GroupVoteCount, Group *> partition_info;
        vector<Strategy> possible_nash_equilibrium;
        StrategyPayOff max_pay_off;

        Strategy(multimap<GroupVoteCount, Group *> partition_info);
    };

    struct Profile{
        Strategy * first_party_strategy;
        Strategy * second_party_strategy;
        StrategyPayOff first_party_payoff;
        StrategyPayOff second_party_payoff;

        Profile(Strategy* first_party_strategy,Strategy* second_party_strategy);
    };

    class Party {
    private:
        map<CandidateId, CandidateVoteCount> candidates_info;
        vector<vector<Group>> groups_combination_info;

        void InitCombinationInfo(int seats_num);

    public:
        Party(map<CandidateId, CandidateVoteCount> candidate_info, int seats_num);
    };


    class Solver {
    private:
        int seats_num;

        Profile ComputePayOff(Strategy *first_party_strategy, Strategy *second_party_strategy);
    };
}
