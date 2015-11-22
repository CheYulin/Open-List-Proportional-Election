//
// Created by yche on 11/20/2015.
//

#include "open_list_solver.h"

using namespace election;

Profile::Profile() : fixed_strategy_payoff_(0), store_strategy_payoff_(0) {

}

//Solver Related
Solver::Solver(Party *first_party, Party *second_party, int seats_num) :
        first_party_(first_party), second_party_(second_party), seats_num_(seats_num) {

}

Solver::~Solver() {
    delete first_party_;
    delete second_party_;
}

Profile election::Solver::ComputePayOff(Strategy *fixed_strategy,
                                        Strategy *stored_strategy) {
    int sum_votes = first_party_->GetSumVotes() + second_party_->GetSumVotes();
    int remaining_seats = seats_num_;
    int quota = sum_votes / seats_num_;
    CompareVoteGroupPriorityQueue fixed_strategy_info = fixed_strategy->groups_combination_info_;
    CompareVoteGroupPriorityQueue stored_strategy_info = stored_strategy->groups_combination_info_;

    Profile profile;

    //First Round with Quota
    while (!fixed_strategy_info.empty() && fixed_strategy_info.top()->group_vote_count_ >= quota) {
        fixed_strategy_info.pop();
        profile.fixed_strategy_payoff_ += 1;
        remaining_seats--;
    }
    while (!stored_strategy_info.empty() && stored_strategy_info.top()->group_vote_count_ >= quota) {
        stored_strategy_info.pop();
        profile.store_strategy_payoff_ += 1;
        remaining_seats--;
    }

    //Next Several Rounds
    while (remaining_seats > 0) {
        int max_vote_fixed_strategy = fixed_strategy_info.empty() ? 0 : fixed_strategy_info.top()->group_vote_count_;
        int max_vote_stored_strategy = stored_strategy_info.empty() ? 0 : stored_strategy_info.top()->group_vote_count_;
        int max_vote = max(max_vote_fixed_strategy, max_vote_stored_strategy);

        int fixed_strategy_pop_num = 0;
        int stored_strategy_pop_num = 0;
        while (!fixed_strategy_info.empty() && fixed_strategy_info.top()->group_vote_count_ == max_vote) {
            fixed_strategy_info.pop();
            fixed_strategy_pop_num++;
        }
        while (!stored_strategy_info.empty() && stored_strategy_info.top()->group_vote_count_ == max_vote) {
            stored_strategy_info.pop();
            stored_strategy_pop_num++;
        }
        int sum_pop_num = fixed_strategy_pop_num + stored_strategy_pop_num;
        if (sum_pop_num > remaining_seats) {
            profile.fixed_strategy_payoff_ += remaining_seats * fixed_strategy_pop_num / (double) sum_pop_num;
            profile.store_strategy_payoff_ += remaining_seats * stored_strategy_pop_num / (double) sum_pop_num;
            remaining_seats = 0;
        }
        else {
            profile.fixed_strategy_payoff_ += fixed_strategy_pop_num;
            profile.store_strategy_payoff_ += stored_strategy_pop_num;
            remaining_seats -= fixed_strategy_pop_num + stored_strategy_pop_num;
        }
    }

    return profile;
}

//Find Nash Equilibrium Related

void Solver::FindNashEquilibrium(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party) {
    vector <SameSizeStrategies> *fixed_different_size_strategies = &fixed_for_traverse_party->getStrategies_with_different_size_();
    vector <SameSizeStrategies> *store_different_size_strategies = &store_nash_equilibrium_party->getStrategies_with_different_size_();
    for (SameSizeStrategies &fixed_same_size_strategies : *fixed_different_size_strategies) {
        for (Strategy &fixed_strategy : fixed_same_size_strategies) {
            TraverseTheOtherPartyStrategies(store_different_size_strategies, &fixed_strategy);
        }
    }
}

void Solver::TraverseTheOtherPartyStrategies(vector <SameSizeStrategies> *store_different_size_strategies,
                                             Strategy *fixed_strategy) {
    vector<Strategy *> possible_to_be_updated_stored_strategies;
    for (SameSizeStrategies &store_same_size_strategies : *store_different_size_strategies) {
        for (Strategy &stored_strategy: store_same_size_strategies) {

            //Two Party Give Partition Less Than Seats Num : Should Be Excluded
            if (fixed_strategy->groups_combination_info_.size() + stored_strategy.groups_combination_info_.size() < seats_num_) {
                break;
            }

            Profile profile = ComputePayOff(fixed_strategy, &stored_strategy);

            //Judge If Party_Store Can Shift Down (Given a Fixed Other Party Strategy)
            if (profile.store_strategy_payoff_ < fixed_strategy->the_other_party_max_pay_off_)
                continue;
            else {
                //Update Store_Strategy_Max_Payoff (Given a Fixed Other Party Strategy)
                //Only When Upper Strategy can Shift to This Guy
                if (profile.store_strategy_payoff_ > fixed_strategy->the_other_party_max_pay_off_) {
                    //Upper Guys can Shift to This Guy
                    possible_to_be_updated_stored_strategies.clear();
                    fixed_strategy->the_other_party_max_pay_off_ = profile.store_strategy_payoff_;
                }

                //Judge If Party_Fixed Can Shift to Left (Compared with Former Max Party_Fixed Payoff)
                if(profile.fixed_strategy_payoff_ < stored_strategy.the_other_party_max_pay_off_){
                    continue;
                }

                //Update Fixed_Strategy_Max_Payoff (Given a certain Stored Strategy)
                //Only When Left Strategy can Shift to This Guy
                if (profile.fixed_strategy_payoff_ > stored_strategy.the_other_party_max_pay_off_) {
                    //Former Assumed Nash Equilibrium Can Shift to This Guy, Need to Remove Former
                    stored_strategy.possible_nash_equilibrium_.clear();
                    stored_strategy.the_other_party_max_pay_off_ = profile.fixed_strategy_payoff_;
                }


                possible_to_be_updated_stored_strategies.push_back(&stored_strategy);
            }
        }
    }

    //Update Possible Nash Equilibrium
    for (Strategy *&to_be_updated_store_strategies :possible_to_be_updated_stored_strategies) {
        to_be_updated_store_strategies->possible_nash_equilibrium_.push_back(fixed_strategy);
    }
}

void Solver::PrintNashEquilibrium() {
    FindNashEquilibrium(first_party_, second_party_);
    vector <SameSizeStrategies> *store_different_size_strategies = &first_party_->getStrategies_with_different_size_();
    std::ios_base::sync_with_stdio(false);
    cout << setiosflags(ios::fixed) << setprecision(2);
    for (SameSizeStrategies &store_same_size_strategies : *store_different_size_strategies) {
        for (Strategy &stored_strategy: store_same_size_strategies) {
            for (Strategy *&fixed_strategy : stored_strategy.possible_nash_equilibrium_) {
                stringstream string_builder;
                string_builder << "({" << stored_strategy.ToString() << "," << fixed_strategy->ToString() << "})" <<
                "\t";
                string_builder << "payoff    " << "(" << fixed_strategy->the_other_party_max_pay_off_ << ",  " <<
                stored_strategy.the_other_party_max_pay_off_ << ")";
                cout << string_builder.str() << endl;
            }

        }
    }


}


