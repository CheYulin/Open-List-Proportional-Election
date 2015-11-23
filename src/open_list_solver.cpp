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

void Solver::UpdateCertainCandidateListFirstRoundSeatNum(vector<CandidateListInfo *> &garbage_collector,
                                                         CompareVoteCandidateListPriorityQueue &strategy_priority_queue,
                                                         StrategyPayOff &strategy_payoff, int &remaining_seats,
                                                         const int quota) {
    int stored_quota_info = strategy_priority_queue.top()->first_round_seat_num_;
    if(stored_quota_info >100){
        cout << "fail to retrive";
    }
    const CandidateListInfo *candidate_list_info = strategy_priority_queue.top();
    //First Time to Query This Candidate List
    if (stored_quota_info == -1) {

        int candidate_list_vote = candidate_list_info->group_vote_count_;
        int candidate_person_num = candidate_list_info->candidates_->size();
        int seat_num_wining_by_quota = min(candidate_list_vote / quota, candidate_person_num);

        //1st Update Winning Seats Num By Quota (Deterministic)
        candidate_list_info->first_round_seat_num_ = seat_num_wining_by_quota;
        //2nd Judge If remains Candidates for Next Round
        int remaining_vote = candidate_list_vote - seat_num_wining_by_quota * quota;
        if (seat_num_wining_by_quota < candidate_person_num && remaining_vote > 0) {
            candidate_list_info->has_candidates_in_list_ = true;
            candidate_list_info->remaining_vote_num = remaining_vote;
        }
    }

    //Then Read Its Value From Past Record
    strategy_payoff += candidate_list_info->first_round_seat_num_;
    remaining_seats -= candidate_list_info->first_round_seat_num_;
    strategy_priority_queue.pop();
    if (candidate_list_info->has_candidates_in_list_) {
        CandidateListInfo *new_temp_candidate_list_info_for_next_rounds = new CandidateListInfo(
                candidate_list_info->remaining_vote_num);
        strategy_priority_queue.push(new_temp_candidate_list_info_for_next_rounds);
        garbage_collector.push_back(new_temp_candidate_list_info_for_next_rounds);
    }

}

Profile election::Solver::ComputePayOff(Strategy *fixed_strategy,
                                        Strategy *stored_strategy) {
    int sum_votes = first_party_->GetSumVotes() + second_party_->GetSumVotes();
    int remaining_seats = seats_num_;
    int quota = sum_votes / seats_num_;
    CompareVoteCandidateListPriorityQueue fixed_strategy_priority_queue = fixed_strategy->groups_combination_info_;
    CompareVoteCandidateListPriorityQueue stored_strategy_priority_queue = stored_strategy->groups_combination_info_;

    Profile profile;
    vector<CandidateListInfo *> garbage_collector;

    //First Round with Quota
    while (!fixed_strategy_priority_queue.empty() && fixed_strategy_priority_queue.top()->group_vote_count_ >= quota) {
        UpdateCertainCandidateListFirstRoundSeatNum(garbage_collector, fixed_strategy_priority_queue,
                                                    profile.fixed_strategy_payoff_, remaining_seats, quota);
    }
    while (!stored_strategy_priority_queue.empty() &&
           stored_strategy_priority_queue.top()->group_vote_count_ >= quota) {
        UpdateCertainCandidateListFirstRoundSeatNum(garbage_collector, stored_strategy_priority_queue,
                                                    profile.store_strategy_payoff_, remaining_seats, quota);
    }

    //Next Several Rounds
    while (remaining_seats > 0 && (!fixed_strategy_priority_queue.empty() || !stored_strategy_priority_queue.empty())) {
        int max_vote_fixed_strategy = fixed_strategy_priority_queue.empty() ? 0
                                                                            : fixed_strategy_priority_queue.top()->group_vote_count_;
        int max_vote_stored_strategy = stored_strategy_priority_queue.empty() ? 0
                                                                              : stored_strategy_priority_queue.top()->group_vote_count_;
        int max_vote = max(max_vote_fixed_strategy, max_vote_stored_strategy);

        int fixed_strategy_pop_num = 0;
        int stored_strategy_pop_num = 0;
        while (!fixed_strategy_priority_queue.empty() &&
               fixed_strategy_priority_queue.top()->group_vote_count_ == max_vote) {
            fixed_strategy_priority_queue.pop();
            fixed_strategy_pop_num++;
        }
        while (!stored_strategy_priority_queue.empty() &&
               stored_strategy_priority_queue.top()->group_vote_count_ == max_vote) {
            stored_strategy_priority_queue.pop();
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

    //Garbage Collection
    for (CandidateListInfo *candidate_list_info :garbage_collector) {
        delete candidate_list_info;
    }
    return profile;
}

//Find Nash Equilibrium Related

void Solver::FindNashEquilibrium(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party) {
    vector<SameSizeStrategies> *fixed_different_size_strategies = &fixed_for_traverse_party->getStrategies_with_different_size_();
    vector<SameSizeStrategies> *store_different_size_strategies = &store_nash_equilibrium_party->getStrategies_with_different_size_();
    for (SameSizeStrategies &fixed_same_size_strategies : *fixed_different_size_strategies) {
        for (Strategy &fixed_strategy : fixed_same_size_strategies) {
            TraverseTheOtherPartyStrategies(store_different_size_strategies, &fixed_strategy);
        }
    }
}

void Solver::TraverseTheOtherPartyStrategies(vector<SameSizeStrategies> *store_different_size_strategies,
                                             Strategy *fixed_strategy) {
    vector<Strategy *> possible_to_be_updated_stored_strategies;
    for (SameSizeStrategies &store_same_size_strategies : *store_different_size_strategies) {
        for (Strategy &stored_strategy: store_same_size_strategies) {

            //Two Party Give Partition Less Than Seats Num : Should Be Excluded
//            if (fixed_strategy->groups_combination_info_.size() + stored_strategy.groups_combination_info_.size() <
//                seats_num_) {
//                break;
//            }

            Profile profile = ComputePayOff(fixed_strategy, &stored_strategy);

            //Update Fixed_Strategy_Max_Payoff (Given a certain Stored Strategy)
            //Only When Left Strategy can Shift to This Guy
            if (profile.fixed_strategy_payoff_ > stored_strategy.the_other_party_max_pay_off_) {
                //Former Assumed Nash Equilibrium Can Shift to This Guy, Need to Remove Former
                stored_strategy.possible_nash_equilibrium_.clear();
                stored_strategy.the_other_party_max_pay_off_ = profile.fixed_strategy_payoff_;
            }

            //Update Store_Strategy_Max_Payoff (Given a Fixed Other Party Strategy)
            //Only When Upper Strategy can Shift to This Guy
            if (profile.store_strategy_payoff_ > fixed_strategy->the_other_party_max_pay_off_) {
                //Upper Guys can Shift to This Guy
                possible_to_be_updated_stored_strategies.clear();
                fixed_strategy->the_other_party_max_pay_off_ = profile.store_strategy_payoff_;
            }

            if (profile.fixed_strategy_payoff_ >= stored_strategy.the_other_party_max_pay_off_ &&
                profile.store_strategy_payoff_ >= fixed_strategy->the_other_party_max_pay_off_) {
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
    vector<SameSizeStrategies> *store_different_size_strategies = &first_party_->getStrategies_with_different_size_();
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



