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
    for (vector<SameSizeStrategies>::iterator same_size_strategy_iterator = store_different_size_strategies->begin();
         same_size_strategy_iterator != store_different_size_strategies->end(); same_size_strategy_iterator++) {
        SameSizeStrategies &store_same_size_strategies = *same_size_strategy_iterator;
        for (SameSizeStrategies::iterator strategy_iterator = store_same_size_strategies.begin();
             strategy_iterator != store_same_size_strategies.end(); strategy_iterator++) {
            Strategy &stored_strategy = *strategy_iterator;

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
    for (vector<Strategy *>::iterator my_iterator = possible_to_be_updated_stored_strategies.begin();
         my_iterator != possible_to_be_updated_stored_strategies.end(); my_iterator++) {
        Strategy *&to_be_updated_store_strategies = *my_iterator;
        to_be_updated_store_strategies->possible_nash_equilibrium_.push_back(fixed_strategy);
    }
}

void Solver::PrintNashEquilibrium() {
    FindNashEquilibrium(first_party_, second_party_);
    vector<SameSizeStrategies> *store_different_size_strategies = &first_party_->getStrategies_with_different_size_();
    std::ios_base::sync_with_stdio(false);
    cout << setiosflags(ios::fixed) << setprecision(2);
    for (vector<SameSizeStrategies>::iterator same_size_strategies_iterator = store_different_size_strategies->begin();
         same_size_strategies_iterator != store_different_size_strategies->end(); same_size_strategies_iterator++) {
        SameSizeStrategies &store_same_size_strategies = *same_size_strategies_iterator;
        for (SameSizeStrategies::iterator strategy_iterator = store_same_size_strategies.begin();
             strategy_iterator != store_same_size_strategies.end(); strategy_iterator++) {
            Strategy &stored_strategy = *strategy_iterator;
            for (vector<Strategy *>::iterator my_iterator = stored_strategy.possible_nash_equilibrium_.begin();
                 my_iterator != stored_strategy.possible_nash_equilibrium_.end(); my_iterator++) {
                Strategy *&fixed_strategy = *my_iterator;
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

//AlphaBetaPruningSolver Related
AlphaBetaPruningSolver::AlphaBetaPruningSolver(Party *first_party, Party *second_party, int seats_num) : Solver(
        first_party, second_party, seats_num) {
    InitBothPartyStrategiesIntoVector();
}

//Init Strategies for Convenience of Reference
void AlphaBetaPruningSolver::InitSinglePartyStrategiesIntoVector(vector<SameSizeStrategies> *party_strategies,
                                                                 vector<Strategy *> &party_strategies_storage) {
    for (vector<SameSizeStrategies>::iterator same_size_strategies_iterator = party_strategies->begin();
         same_size_strategies_iterator != party_strategies->end(); same_size_strategies_iterator++) {
        SameSizeStrategies &same_party_strategies = *same_size_strategies_iterator;
        for (vector<Strategy>::iterator strategy_iterator = same_party_strategies.begin();
             strategy_iterator != same_party_strategies.end(); strategy_iterator++) {
            party_strategies_storage.push_back(&(*strategy_iterator));
        }
    }
}

void AlphaBetaPruningSolver::InitBothPartyStrategiesIntoVector() {
    InitSinglePartyStrategiesIntoVector(&(getFirst_party_()->getStrategies_with_different_size_()),
                                        first_party_strategies_);
    InitSinglePartyStrategiesIntoVector(&(getSecond_party_()->getStrategies_with_different_size_()),
                                        second_party_strategies_);
}

//AlphaBetaPruningSolverNaive Related
AlphaBetaPruningSolverNaive::AlphaBetaPruningSolverNaive(Party *first_party, Party *second_party, int seats_num)
        : AlphaBetaPruningSolver(first_party, second_party, seats_num) {
    possible_to_be_nash_equilibrium_first_alpha_strategies.clear();
    possible_to_be_nash_equilibrium_second_alpha_strategies.clear();
}

int AlphaBetaPruningSolverNaive::TraverseUsingPruning(vector<Strategy *> &beta_strategies,
                                                      vector<Strategy *> &alpha_strategies,
                                                      vector<Strategy *> &possible_nash_alpha_strategies) {
    // max of minimals
    int max_alpha = -1;
    for (int i = 0; i < alpha_strategies.size(); i++) {
        Strategy *alpha_strategy = alpha_strategies[i];
        int min_value = TraverseBetaStrategies(beta_strategies, alpha_strategy, max_alpha);
        if (min_value > max_alpha) {
            for (Strategy *alpha_strategy : possible_nash_alpha_strategies) {
                alpha_strategy->possible_nash_equilibrium_.clear();
            }
            possible_nash_alpha_strategies.clear();
            max_alpha = min_value;
        }

        if (min_value >= max_alpha) {
            possible_nash_alpha_strategies.push_back(alpha_strategy);
        }
    }
    return max_alpha;
}

int AlphaBetaPruningSolverNaive::TraverseBetaStrategies(vector<Strategy *> &beta_strategies,
                                                        Strategy *alpha_strategy, int &max_of_minimals) {
    vector<Strategy *> &possible_to_be_nash_equilibrium_beta_strategies = alpha_strategy->possible_nash_equilibrium_;
    int min_of_beta_values = 100;
    for (int j = 0; j < beta_strategies.size(); j++) {
        Strategy *beta_strategy = beta_strategies[j];
        Profile profile = ComputePayOff(alpha_strategy, beta_strategy);
        int alpha_value = profile.fixed_strategy_payoff_;

        if (alpha_value < min_of_beta_values) {
            possible_to_be_nash_equilibrium_beta_strategies.clear();
            min_of_beta_values = alpha_value;
            if (min_of_beta_values < max_of_minimals) {
                alpha_strategy->possible_nash_equilibrium_.clear();
                return min_of_beta_values;
            }
        }

        if (alpha_value <= min_of_beta_values) {
            possible_to_be_nash_equilibrium_beta_strategies.push_back(beta_strategy);
        }
    }
    return min_of_beta_values;
}

void AlphaBetaPruningSolverNaive::InitSinglePartyMaximumPossibleNashStrategiesIdMap(
        vector<Strategy *> &single_party_possible_nash_strategies,
        map<Strategy *, int> &possible_nash_alpha_strategies_id_map) {
    int id_number = 0;
    possible_nash_alpha_strategies_id_map.clear();
    for (int i = 0; i < single_party_possible_nash_strategies.size(); i++) {
        Strategy *strategy = single_party_possible_nash_strategies[i];
        possible_nash_alpha_strategies_id_map.insert(make_pair(strategy, id_number));
        id_number++;
    }
}

void AlphaBetaPruningSolverNaive::PrintNashEquilibrium() {
    cout << time(NULL) << endl;
    cout << time(NULL) << "Finish Init" << endl;
    int second_party_alpha_max1 = TraverseUsingPruning(first_party_strategies_, second_party_strategies_,
                                                       possible_to_be_nash_equilibrium_first_alpha_strategies);
    cout << time(NULL) << "Finish First" << endl;
    int first_party_alpha_max2 = TraverseUsingPruning(second_party_strategies_, first_party_strategies_,
                                                      possible_to_be_nash_equilibrium_second_alpha_strategies);
    cout << time(NULL) << "Finish Second" << endl;
    vector<Strategy *> &first_party_possible_nash_strategies = possible_to_be_nash_equilibrium_second_alpha_strategies;
    vector<Strategy *> &second_party_possible_nash_strategies = possible_to_be_nash_equilibrium_first_alpha_strategies;

    InitSinglePartyMaximumPossibleNashStrategiesIdMap(first_party_possible_nash_strategies,
                                                      first_party_possible_nash_alpha_strategies_id_map);
    cout << time(NULL) << "Finish First Result Init" << endl;
    InitSinglePartyMaximumPossibleNashStrategiesIdMap(second_party_possible_nash_strategies,
                                                      second_party_possible_nash_alpha_strategies_id_map);
    cout << time(NULL) << "Finish Second Result Init" << endl;
    size_t first_party_size = first_party_possible_nash_strategies.size();
    size_t second_party_size = second_party_possible_nash_strategies.size();
    size_t all_possible_nash_profile_size = first_party_size * second_party_size / BYTE_SIZE + 1;
    unsigned char *possible_nash_strategies_bitmap = new unsigned char[all_possible_nash_profile_size];
    memset(possible_nash_strategies_bitmap, 0, all_possible_nash_profile_size);

    cout << time(NULL) << endl;

    //First Time
    for (size_t row_num = 0; row_num < first_party_possible_nash_strategies.size(); row_num++) {
        Strategy *first_party_strategy = first_party_possible_nash_strategies[row_num];
        for (vector<Strategy *>::iterator strategy_iterator = first_party_strategy->possible_nash_equilibrium_.begin();
             strategy_iterator != first_party_strategy->possible_nash_equilibrium_.end(); strategy_iterator++) {
            if (second_party_possible_nash_alpha_strategies_id_map.find(*strategy_iterator) !=
                second_party_possible_nash_alpha_strategies_id_map.end()) {
                size_t col_num = second_party_possible_nash_alpha_strategies_id_map[*strategy_iterator];
                size_t bit_index = row_num * second_party_size + col_num;
                size_t char_index = bit_index / BYTE_SIZE;
                int index_in_eight_bits = bit_index % BYTE_SIZE;
                unsigned char tmp_char = 0x01;
                tmp_char << (index_in_eight_bits);
                possible_nash_strategies_bitmap[char_index] |= tmp_char;
            }
        }
    }
    //Second Time Check And Print
    for (size_t col_num = 0; col_num < second_party_possible_nash_strategies.size(); col_num++) {
        Strategy *second_party_strategy = second_party_possible_nash_strategies[col_num];
        for (vector<Strategy *>::iterator strategy_iterator = second_party_strategy->possible_nash_equilibrium_.begin();
             strategy_iterator != second_party_strategy->possible_nash_equilibrium_.end(); strategy_iterator++) {
            if (first_party_possible_nash_alpha_strategies_id_map.find(*strategy_iterator) !=
                first_party_possible_nash_alpha_strategies_id_map.end()) {
                size_t row_num = first_party_possible_nash_alpha_strategies_id_map[*strategy_iterator];
                size_t bit_index = row_num * second_party_size + col_num;
                size_t char_index = bit_index / BYTE_SIZE;
                int index_in_eight_bits = bit_index % BYTE_SIZE;
                unsigned char tmp_char = 0x01;
                tmp_char << (index_in_eight_bits);
                if (tmp_char & possible_nash_strategies_bitmap[char_index] == tmp_char) {
                    //Print
                    Strategy *first_party_strategy = *strategy_iterator;
                    stringstream string_builder;
                    string_builder << "({" << first_party_strategy->ToString() << "," <<
                    second_party_strategy->ToString() << "})" <<
                    "\t";
                    string_builder << "payoff    " << "(" << first_party_alpha_max2 << ",  " <<
                    second_party_alpha_max1 << ")";
                    cout << string_builder.str() << endl;

                }
            }
        }
    }

    delete possible_nash_strategies_bitmap;
}

//AlphaBetaPruningSolverWithBits Related
AlphaBetaPruningSolverWithBits::AlphaBetaPruningSolverWithBits(Party *first_party, Party *second_party, int seats_num)
        : AlphaBetaPruningSolver(first_party, second_party, seats_num) {
    size_t first_party_size = first_party_strategies_.size();
    size_t second_party_size = second_party_strategies_.size();
    size_t all_profile_size = first_party_size * second_party_size;
    size_t char_size = all_profile_size / BYTE_SIZE + 1;
    first_alpha_possible_nash_bitmap = new unsigned char[char_size];
    second_alpha_possible_nash_bitmap = new unsigned char[char_size];
}

void AlphaBetaPruningSolverWithBits::PrintNashEquilibrium() {
    //second_party_strategies_ as Row
    int second_party_alpha_max = TraverseUsingPruning(first_party_strategies_,second_party_strategies_,first_alpha_possible_nash_bitmap);
    int first_party_alpha_max =TraverseUsingPruning(second_party_strategies_,first_party_strategies_,second_alpha_possible_nash_bitmap);
    unsigned char *& second_party_as_row_profiles = first_alpha_possible_nash_bitmap;
    unsigned char *& first_party_as_row_profiles =second_alpha_possible_nash_bitmap;
    size_t first_party_size = first_party_strategies_.size();
    size_t second_party_size = second_party_strategies_.size();
    size_t all_profile_size = first_party_size * second_party_size;
    for(size_t first_party_bit_index =0; first_party_bit_index < all_profile_size; first_party_bit_index++){
        size_t first_party_as_row_char_index = first_party_bit_index / BYTE_SIZE;
        size_t first_party_as_row_index_in_eight_bits = first_party_bit_index % BYTE_SIZE;

        size_t first_party_row_num = first_party_bit_index / second_party_size;
        size_t first_party_col_num = first_party_bit_index % second_party_size;

        size_t second_party_col_num =first_party_row_num;
        size_t second_party_row_num=first_party_col_num;
        size_t second_party_bit_index = second_party_row_num * first_party_size + second_party_col_num;
        size_t second_party_as_row_char_index = second_party_bit_index/BYTE_SIZE;
        size_t second_party_as_row_index_in_eight_bits=second_party_bit_index%BYTE_SIZE;
        unsigned char tmp_char = 0x01;
        unsigned char first_party_with_one_bit = tmp_char & (first_party_as_row_profiles[first_party_as_row_char_index] >> first_party_as_row_index_in_eight_bits);
        unsigned char second_party_with_one_bit = tmp_char & (second_party_as_row_profiles[second_party_as_row_char_index] >> second_party_as_row_index_in_eight_bits);
        if(first_party_with_one_bit & second_party_with_one_bit == tmp_char){
            //Print
            Strategy *first_party_strategy = first_party_strategies_[first_party_row_num];
            Strategy *second_party_strategy = second_party_strategies_[first_party_col_num];
            stringstream string_builder;
            string_builder << "({" << first_party_strategy->ToString() << "," <<
            second_party_strategy->ToString() << "})" <<
            "\t";
            string_builder << "payoff    " << "(" << first_party_alpha_max << ",  " <<
            second_party_alpha_max << ")";
            cout << string_builder.str() << endl;
        }
    }
}

int AlphaBetaPruningSolverWithBits::TraverseUsingPruning(vector<Strategy *> &beta_strategies,
                                                         vector<Strategy *> &alpha_strategies,
                                                         unsigned char *&alpha_possible_nash_bitmap) {
//    vector<int> po
    // max of minimals
    int max_alpha = -1;
    for (int row_num = 0; row_num < alpha_strategies.size(); row_num++) {
        Strategy *alpha_strategy = alpha_strategies[row_num];
        int min_value = TraverseBetaStrategies(beta_strategies, alpha_strategy, alpha_possible_nash_bitmap, max_alpha,
                                               row_num);
        if (min_value > max_alpha) {
            //Need Clear Before

            max_alpha = min_value;
        }
    }
    return max_alpha;
}

int AlphaBetaPruningSolverWithBits::TraverseBetaStrategies(vector<Strategy *> &beta_strategies,
                                                           Strategy *alpha_strategy, unsigned char *&alpha_possible_nash_bitmap,
                                                           int &max_of_minimals, int row_num) {
    vector<int> possible_to_be_nash_equilibrium_beta_strategies;
    int min_of_beta_values = 100;
    for (int col_num = 0; col_num < beta_strategies.size(); col_num++) {
        Strategy *beta_strategy = beta_strategies[col_num];
        Profile profile = ComputePayOff(alpha_strategy, beta_strategy);
        int alpha_value = profile.fixed_strategy_payoff_;

        if (alpha_value < min_of_beta_values) {
            possible_to_be_nash_equilibrium_beta_strategies.clear();
            min_of_beta_values = alpha_value;
            if (min_of_beta_values < max_of_minimals) {
                return min_of_beta_values;
            }
        }

        if (alpha_value <= min_of_beta_values) {
            possible_to_be_nash_equilibrium_beta_strategies.push_back(col_num);
        }
    }

    size_t beta_strategies_size = beta_strategies.size();

    for (vector<int>::iterator strategy_iterator = possible_to_be_nash_equilibrium_beta_strategies.begin();
         strategy_iterator != possible_to_be_nash_equilibrium_beta_strategies.end(); strategy_iterator++) {
        size_t col_num = *strategy_iterator;
        size_t bit_index = row_num * beta_strategies_size + col_num;
        size_t char_index = bit_index / BYTE_SIZE;
        int index_in_eight_bits = bit_index % BYTE_SIZE;
        unsigned char tmp_char = 0x01;
        tmp_char << (index_in_eight_bits);
        alpha_possible_nash_bitmap[char_index] |= tmp_char;
    }
    return min_of_beta_values;
}

AlphaBetaPruningSolverWithBits::~AlphaBetaPruningSolverWithBits() {
    delete first_alpha_possible_nash_bitmap;
    delete second_alpha_possible_nash_bitmap;
}
