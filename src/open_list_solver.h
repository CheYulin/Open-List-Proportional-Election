//
// Created by yche on 11/20/2015.
//

#ifndef SRC_OPEN_LIST_SOLVER_H
#define SRC_OPEN_LIST_SOLVER_H

#endif //SRC_OPEN_LIST_SOLVER_H

#include "open_list_party.h"

using namespace election;

namespace election {
    typedef double StrategyPayOff;

    struct Strategy {
        priority_queue<Group, vector<Group>, GroupComparePartition> groups_info_;
        vector<Strategy> possible_nash_equilibrium_;
        StrategyPayOff max_pay_off_;

    };

    struct Profile {
        Strategy *first_party_strategy_;
        Strategy *second_party_strategy_;
        StrategyPayOff first_party_payoff_;
        StrategyPayOff second_party_payoff_;

        Profile(Strategy *first_party_strategy, Strategy *second_party_strategy);
    };

    class Solver {
    private:
        int seats_num_;
        Party *first_party_;
        Party *second_party_;

        Profile ComputePayOff(Strategy *first_party_strategy, Strategy *second_party_strategy);

    public:
        Solver(Party *first_party, Party *second_party, int seats_num);

        ~Solver();

        //Auto Generated
        Party *getFirst_party_() const {
            return first_party_;
        }

        Party *getSecond_party_() const {
            return second_party_;
        }
    };
}