//
// Created by yche on 11/20/2015.
//

#ifndef SRC_OPEN_LIST_SOLVER_H
#define SRC_OPEN_LIST_SOLVER_H

#endif //SRC_OPEN_LIST_SOLVER_H

#include "open_list_party.h"


namespace election {

    struct Profile {
        VoteNumber left_strategy_payoff_;
        VoteNumber right_strategy_payoff_;

        Profile();
    };

    struct SolverInfo{
        Party* first_party_;
        Party* second_party_;
        int seat_num;
    };

    class Solver {
    private:
        int seats_num_;
        int quota;
        Party *first_party_;
        Party *second_party_;

        void InitStrategyInformation(Strategy *strategy);

        void InitStrategiesForSingleParty(Party *party);

        void InitTwoPartiesStrategies();

        void TraverseTheOtherPartyStrategies(vector<SameSizeStrategies> *store_different_size_strategies,
                                             Strategy *fixed_strategy);

        virtual void FindNashEquilibrium(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party);

    public:
        Solver(Party *first_party, Party *second_party, int seats_num);

        virtual ~Solver();

        Profile ComputePayOff(Strategy *left_strategy, Strategy *right_strategy);

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
    protected:
        vector<Strategy *> first_party_strategies_;
        vector<Strategy *> second_party_strategies_;

        void InitSinglePartyStrategiesIntoVector(vector<SameSizeStrategies> *party_strategies,
                                                 vector<Strategy *> &party_strategies_storage);

        void InitBothPartyStrategiesIntoVector();

    public:
        AlphaBetaPruningSolver(Party *first_party, Party *second_party, int seats_num);

        virtual void PrintNashEquilibrium() = 0;

    };

    class AlphaBetaPruningSolverNaive : public AlphaBetaPruningSolver {
    private :
        vector<Strategy *> possible_to_be_nash_equilibrium_first_alpha_strategies;
        vector<Strategy *> possible_to_be_nash_equilibrium_second_alpha_strategies;
        map<Strategy *, int> first_party_possible_nash_alpha_strategies_id_map;
        map<Strategy *, int> second_party_possible_nash_alpha_strategies_id_map;

        VoteNumber TraverseBetaStrategies(vector<Strategy *> &beta_strategies, Strategy *alpha_strategy,
                                   VoteNumber &max_of_minimals);

        VoteNumber TraverseUsingPruning(vector<Strategy *> &beta_strategies, vector<Strategy *> &alpha_strategies,
                                 vector<Strategy *> &possible_nash_alpha_strategies);

    public:
        void InitSinglePartyMaximumPossibleNashStrategiesIdMap(
                vector<Strategy *> &single_party_possible_nash_strategies,
                map<Strategy *, int> &possible_nash_alpha_strategies_id_map);

        AlphaBetaPruningSolverNaive(Party *first_party, Party *second_party, int seats_num);

        virtual void PrintNashEquilibrium();
    };

    class AlphaBetaPruningSolverWithBits : public AlphaBetaPruningSolver {
    private:
        unsigned char *first_alpha_possible_nash_bitmap;
        unsigned char *second_alpha_possible_nash_bitmap;

        VoteNumber TraverseBetaStrategies(vector<Strategy *> &beta_strategies, Strategy *alpha_strategy,
                                   unsigned char *&alpha_possible_nash_bitmap, VoteNumber &max_of_minimals, int row_num);

        VoteNumber TraverseUsingPruning(vector<Strategy *> &beta_strategies, vector<Strategy *> &alpha_strategies,
                                 unsigned char *&alpha_possible_nash_bitmap);

    public:
        AlphaBetaPruningSolverWithBits(Party *first_party, Party *second_party, int seats_num);

        virtual void PrintNashEquilibrium();

        ~AlphaBetaPruningSolverWithBits();
    };
}