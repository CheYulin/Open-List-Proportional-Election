//
// Created by yche on 11/20/2015.
//

#ifndef SRC_OPEN_LIST_SOLVER_H
#define SRC_OPEN_LIST_SOLVER_H

#endif //SRC_OPEN_LIST_SOLVER_H

#include "open_list_party.h"


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

        void UpdateCertainCandidateListFirstRoundSeatNum(vector<CandidateListInfo *> &garbage_collector,
                                                         CompareVoteCandidateListPriorityQueue &strategy_priority_queue,
                                                         StrategyPayOff &strategy_payoff, int &remaining_seats,
                                                         const int quota);

        void TraverseTheOtherPartyStrategies(vector<SameSizeStrategies> *store_different_size_strategies,
                                             Strategy *fixed_strategy);

        virtual void FindNashEquilibrium(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party);

    public:
        Solver(Party *first_party, Party *second_party, int seats_num);

        virtual ~Solver();

        Profile ComputePayOff(Strategy *fixed_strategy, Strategy *stored_strategy);

        virtual void PrintNashEquilibrium();

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

    class AlphaBetaPruningSolver : public Solver {
    private:
        vector<Strategy *> first_party_strategies_;
        vector<Strategy *> second_party_strategies_;
        vector<Strategy *> possible_to_be_nash_equilibrium_first_alpha_strategies;
        vector<Strategy *> possible_to_be_nash_equilibrium_second_alpha_strategies;

        void InitSinglePartyStrategiesIntoVector(vector<SameSizeStrategies> *party_strategies,
                                                 vector<Strategy *> &party_strategies_storage);

        map<Strategy *, int> first_party_possible_nash_alpha_strategies_id_map;
        map<Strategy *, int> second_party_possible_nash_alpha_strategies_id_map;

        void InitBothPartyStrategiesIntoVector();


        int TraverseBetaStrategies(vector<Strategy *> &beta_strategies, Strategy *alpha_strategy,
                                   int &max_of_minimals);

        int TraverseUsingPruning(vector<Strategy *> &beta_strategies, vector<Strategy *> &alpha_strategies,
                                 vector<Strategy *> &possible_nash_alpha_strategies);

        void InitSinglePartyMaximumPossibleNashStrategiesIdMap(vector<Strategy *> &single_party_possible_nash_strategies,
                                                               map<Strategy *, int> &possible_nash_alpha_strategies_id_map);

    public:

        AlphaBetaPruningSolver(Party *first_party, Party *second_party, int seats_num);

        virtual void PrintNashEquilibrium();

    };
}