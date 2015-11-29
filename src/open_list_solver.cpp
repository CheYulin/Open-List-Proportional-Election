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
    if (stored_quota_info > 100) {
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
    for (vector<CandidateListInfo *>::iterator my_iterator = garbage_collector.begin();
         my_iterator != garbage_collector.end(); my_iterator++) {
        CandidateListInfo *candidate_list_info = *my_iterator;
        delete candidate_list_info;
    }

    return profile;
}

//Find Nash Equilibrium Related

void Solver::FindNashEquilibrium(Party *store_nash_equilibrium_party, Party *fixed_for_traverse_party) {
    vector<SameSizeStrategies> *fixed_different_size_strategies = &fixed_for_traverse_party->getStrategies_with_different_size_();
    vector<SameSizeStrategies> *store_different_size_strategies = &store_nash_equilibrium_party->getStrategies_with_different_size_();
    for (vector<SameSizeStrategies>::iterator same_size_partition_iterator = fixed_different_size_strategies->begin();
         same_size_partition_iterator != fixed_different_size_strategies->end(); same_size_partition_iterator++) {
        SameSizeStrategies &fixed_same_size_strategies = *same_size_partition_iterator;
        for (SameSizeStrategies::iterator strategy_iterator = same_size_partition_iterator->begin();
             strategy_iterator != same_size_partition_iterator->end(); strategy_iterator++) {
            Strategy &fixed_strategy = *strategy_iterator;
            TraverseTheOtherPartyStrategies(store_different_size_strategies, &fixed_strategy);
        }
    }
}

void Solver::TraverseTheOtherPartyStrategies(vector<SameSizeStrategies> *store_different_size_strategies,
                                             Strategy *fixed_strategy) {
    vector<Strategy *> possible_to_be_updated_stored_strategies;
    for(vector<SameSizeStrategies>::iterator same_size_strategy_iterator = store_different_size_strategies->begin();same_size_strategy_iterator!=store_different_size_strategies->end();same_size_strategy_iterator++){
        SameSizeStrategies &store_same_size_strategies = * same_size_strategy_iterator;
        for(SameSizeStrategies::iterator strategy_iterator =store_same_size_strategies.begin(); strategy_iterator!=store_same_size_strategies.end();strategy_iterator++){
            Strategy & stored_strategy = *strategy_iterator;

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
    for(vector<Strategy*>::iterator my_iterator = possible_to_be_updated_stored_strategies.begin(); my_iterator!= possible_to_be_updated_stored_strategies.end();my_iterator++){
        Strategy *& to_be_updated_store_strategies =*my_iterator;
        to_be_updated_store_strategies->possible_nash_equilibrium_.push_back(fixed_strategy);
    }
}

void Solver::PrintNashEquilibrium() {
    FindNashEquilibrium(first_party_, second_party_);
    vector<SameSizeStrategies> *store_different_size_strategies = &first_party_->getStrategies_with_different_size_();
    std::ios_base::sync_with_stdio(false);
    cout << setiosflags(ios::fixed) << setprecision(2);
    for(vector<SameSizeStrategies>::iterator same_size_strategies_iterator = store_different_size_strategies->begin(); same_size_strategies_iterator!=store_different_size_strategies->end();same_size_strategies_iterator++){
        SameSizeStrategies & store_same_size_strategies = *same_size_strategies_iterator;
        for(SameSizeStrategies::iterator strategy_iterator=store_same_size_strategies.begin();strategy_iterator!=store_same_size_strategies.end();strategy_iterator++){
            Strategy&stored_strategy = *strategy_iterator;
            for(vector<Strategy*>::iterator my_iterator=stored_strategy.possible_nash_equilibrium_.begin(); my_iterator!=stored_strategy.possible_nash_equilibrium_.end();my_iterator++){
                Strategy *&fixed_strategy =* my_iterator;
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




int AlphaBetaPruningSolver::TraverseUsingPruning(vector<SameSizeStrategies> *beta_strategies,
                                                  vector<SameSizeStrategies> *alpha_strategies) {

    // max of mins
    int max_alpha = -1;
    vector<Strategy *> possible_to_be_nash_equilibrium_alpha_strategies;
    for (SameSizeStrategies &alpha_same_size_strategies : *alpha_strategies) {
        for (Strategy &alpha_strategy : alpha_same_size_strategies) {
            int min_value = TraverseBetaStrategies(beta_strategies, &alpha_strategy, max_alpha);
            if(min_value > max_alpha){
                for(Strategy * alpha_strategy : possible_to_be_nash_equilibrium_alpha_strategies){
                    alpha_strategy->is_possible_nash_equilibrium =false;
                    alpha_strategy->possible_nash_equilibrium_.clear();
                }
                possible_to_be_nash_equilibrium_alpha_strategies.clear();
                max_alpha = min_value;
            }

            if(min_value >= max_alpha){
                alpha_strategy.is_possible_nash_equilibrium = true;
                possible_to_be_nash_equilibrium_alpha_strategies.push_back(&alpha_strategy);
            }

        }
    }
    return max_alpha;
}

int AlphaBetaPruningSolver::TraverseBetaStrategies(vector<SameSizeStrategies> *beta_strategies, Strategy *alpha_strategy, int &max_of_mins) {
    vector<Strategy *> possible_to_be_nash_equilibrium_beta_strategies;
    int min_of_beta_values = 100;
    for (SameSizeStrategies &beta_same_size_strategies : *beta_strategies) {
        for (Strategy &beta_strategy: beta_same_size_strategies) {
            Profile profile = ComputePayOff(alpha_strategy, &beta_strategy);
            int alpha_value = profile.fixed_strategy_payoff_;

            if(alpha_value < min_of_beta_values){
                for(Strategy* beta_strategy : possible_to_be_nash_equilibrium_beta_strategies){
                    beta_strategy->is_possible_nash_equilibrium =false;
                }
                possible_to_be_nash_equilibrium_beta_strategies.clear();
                min_of_beta_values = alpha_value;
                if(min_of_beta_values < max_of_mins){
                    return min_of_beta_values;
                }

            }

            if(alpha_value <= min_of_beta_values){
                beta_strategy.is_possible_nash_equilibrium = true;
                possible_to_be_nash_equilibrium_beta_strategies.push_back(&beta_strategy);
            }

        }
    }

    alpha_strategy->possible_nash_equilibrium_ = possible_to_be_nash_equilibrium_beta_strategies;
    return min_of_beta_values;

}



void AlphaBetaPruningSolver::PrintNashEquilibrium() {
//    Solver::PrintNashEquilibrium();
    int alpha_max1 = TraverseUsingPruning(&getFirst_party_()->getStrategies_with_different_size_(),&getSecond_party_()->getStrategies_with_different_size_());
//    int alpha_max2 = TraverseUsingPruning(&getSecond_party_()->getStrategies_with_different_size_(),&getFirst_party_()->getStrategies_with_different_size_());
    for (SameSizeStrategies &second_party_same_size_strategies : getSecond_party_()->getStrategies_with_different_size_()) {
        for (Strategy &stored_strategy: second_party_same_size_strategies) {
            if(stored_strategy.is_possible_nash_equilibrium == true){
                for (Strategy *&fixed_strategy : stored_strategy.possible_nash_equilibrium_) {
                    if(fixed_strategy->is_possible_nash_equilibrium == true)
                    {
                        stringstream string_builder;
                        string_builder << "({" << stored_strategy.ToString() << "," << fixed_strategy->ToString() << "})" <<
                        "\t";
                        string_builder << "payoff    " << "(" <<(getSeats_num_()-alpha_max1) << ",  " <<
                                alpha_max1   << ")";
                        cout << string_builder.str() << endl;
                    }

                }
            }


        }
    }
}

AlphaBetaPruningSolver::AlphaBetaPruningSolver(Party *first_party, Party *second_party, int seats_num):Solver(first_party,second_party,seats_num) {

}
