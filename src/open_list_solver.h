//
// Created by yche on 11/20/2015.
//

#ifndef SRC_OPEN_LIST_SOLVER_H
#define SRC_OPEN_LIST_SOLVER_H

#endif //SRC_OPEN_LIST_SOLVER_H

#include "open_list_party.h"

using namespace election;

namespace election {

    struct Profile {
        StrategyPayOff fixed_strategy_payoff_;
        StrategyPayOff store_strategy_payoff_;

        Profile();
    };

    class Solver {
    private:
        int seats_num_;
        Party *first_party_;
        Party *second_party_;

        void TraverseTheOtherPartyStrategies(vector<SameSizeStrategies> *store_different_size_strategies,
                                             Strategy *fixed_strategy);

        virtual void FindNashEquilibrium(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party);

    public:
        Solver(Party *first_party, Party *second_party, int seats_num);

        virtual ~Solver();

        Profile ComputePayOff(Strategy *fixed_strategy, Strategy *stored_strategy);

        void PrintNashEquilibrium();

        //Auto Generated
        Party *getFirst_party_() const {
            return first_party_;
        }

        Party *getSecond_party_() const {
            return second_party_;
        }

        int getSeats_num_() const {
            return seats_num_;
        }
    };
}