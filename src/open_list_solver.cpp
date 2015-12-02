//
// Created by yche on 11/20/2015.
//

#include "open_list_solver.h"

using namespace election;

Profile::Profile() : left_strategy_payoff_(0), right_strategy_payoff_(0) {
}

//Solver Related
Solver::Solver(Party *first_party, Party *second_party, int seats_num) :
        first_party_(first_party), second_party_(second_party), seats_num_(seats_num) {
    int sum_votes = first_party_->GetSumVotes() + second_party_->GetSumVotes();
    quota = sum_votes / seats_num_;
    InitTwoPartiesStrategies();
}

Solver::~Solver() {
    delete first_party_;
    delete second_party_;
}

void Solver::InitStrategyInformation(Strategy *strategy) {
    //Guarantee Initialization Correctness
    strategy->fixed_seats_num = 0;
    strategy->remaining_candidate_vote_ordered_list_.clear();

    for (vector<const CandidateListInfo *>::iterator candidate_list_info_iterator = strategy->candidate_list_info_list_.begin();
         candidate_list_info_iterator != strategy->candidate_list_info_list_.end(); candidate_list_info_iterator++) {
        const CandidateListInfo *candidate_list_info = *candidate_list_info_iterator;

        //If Candidate_List_Info Not Initialized
        if (candidate_list_info->first_round_fixed_seat_num_ == -1) {
            int candidate_list_vote = candidate_list_info->group_vote_count_;
            int candidate_person_num = candidate_list_info->candidates_->size();
            int seat_num_wining_by_quota = min(candidate_list_vote / quota, candidate_person_num);

            //1st Update Winning Seats Num By Quota (Deterministic)
            candidate_list_info->first_round_fixed_seat_num_ = seat_num_wining_by_quota;
            //2nd Judge If remains Candidates for Next Round
            int remaining_vote = candidate_list_vote - seat_num_wining_by_quota * quota;
            if (seat_num_wining_by_quota < candidate_person_num && remaining_vote > 0) {
                candidate_list_info->has_candidates_in_list_ = true;
                candidate_list_info->remaining_vote_num = remaining_vote;
            }
        }

        //First Calculate Fixed Seat Number
        strategy->fixed_seats_num += candidate_list_info->first_round_fixed_seat_num_;
        //Second Add To Remains
        if (candidate_list_info->first_round_fixed_seat_num_ == 0) {
            strategy->remaining_candidate_vote_ordered_list_.push_back(candidate_list_info->remaining_vote_num);
        }
        else if (candidate_list_info->has_candidates_in_list_ == true) {
            strategy->remaining_candidate_vote_ordered_list_.push_back(candidate_list_info->remaining_vote_num);
        }
    }

    //Keep Ordered List For Remains
    sort(strategy->remaining_candidate_vote_ordered_list_.begin(),
         strategy->remaining_candidate_vote_ordered_list_.end(), greater<int>());
}

void Solver::InitStrategiesForSingleParty(Party *party) {
    vector<SameSizeStrategies> &different_size_strategies = party->getStrategies_with_different_size_();
    for (vector<SameSizeStrategies>::iterator same_size_strategies_iterator = different_size_strategies.begin();
         same_size_strategies_iterator != different_size_strategies.end(); same_size_strategies_iterator++) {
        SameSizeStrategies &same_size_strategies = *same_size_strategies_iterator;
        for (SameSizeStrategies::iterator strategy_iterator = same_size_strategies.begin();
             strategy_iterator != same_size_strategies.end(); strategy_iterator++) {
            Strategy &strategy = *strategy_iterator;
            InitStrategyInformation(&strategy);
        }
    }
}

void Solver::InitTwoPartiesStrategies() {
    InitStrategiesForSingleParty(first_party_);
    InitStrategiesForSingleParty(second_party_);
}

Profile election::Solver::ComputePayOff(Strategy *left_strategy,
                                        Strategy *right_strategy) {
    Profile profile;
    //First Round Fixed Seat
    profile.left_strategy_payoff_ += left_strategy->fixed_seats_num;
    profile.right_strategy_payoff_ += right_strategy->fixed_seats_num;

    //Next Several Rounds
    int remaining_seat_num = seats_num_ - profile.left_strategy_payoff_ - profile.right_strategy_payoff_;

    int left_remains_index = 0;
    int right_remains_index = 0;
    vector<int> &left_remains = left_strategy->remaining_candidate_vote_ordered_list_;
    vector<int> &right_remains = right_strategy->remaining_candidate_vote_ordered_list_;
    while (remaining_seat_num > 0) {
        int max_remaining_vote_left_strategy =
                left_remains_index >= left_remains.size() ? 0 : left_remains[left_remains_index];
        int max_remaining_vote_right_strategy =
                right_remains_index >= right_remains.size() ? 0 : right_remains[right_remains_index];

        int max_remaining_vote = max(max_remaining_vote_left_strategy, max_remaining_vote_right_strategy);

        int left_strategy_pop_num = 0;
        int right_strategy_pop_num = 0;

        while (left_remains_index < left_remains.size() && left_remains[left_remains_index] == max_remaining_vote) {
            left_strategy_pop_num++;
            left_remains_index++;
        }
        while (right_remains_index < right_remains.size() &&
               right_remains[right_remains_index] == max_remaining_vote) {
            right_strategy_pop_num++;
            right_remains_index++;
        }

        int sum_pop_num = left_strategy_pop_num + right_strategy_pop_num;
        if (sum_pop_num > remaining_seat_num) {
            profile.left_strategy_payoff_ += remaining_seat_num * left_strategy_pop_num / (double) sum_pop_num;
            profile.right_strategy_payoff_ += remaining_seat_num * right_strategy_pop_num / (double) sum_pop_num;
            remaining_seat_num = 0;
        }
        else {
            profile.left_strategy_payoff_ += left_strategy_pop_num;
            profile.right_strategy_payoff_ += right_strategy_pop_num;
            remaining_seat_num -= left_strategy_pop_num + right_strategy_pop_num;
        }
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
            if (profile.left_strategy_payoff_ > stored_strategy.the_other_party_max_pay_off_) {
                //Former Assumed Nash Equilibrium Can Shift to This Guy, Need to Remove Former
                stored_strategy.possible_nash_equilibrium_.clear();
                stored_strategy.the_other_party_max_pay_off_ = profile.left_strategy_payoff_;
            }

            //Update Store_Strategy_Max_Payoff (Given a Fixed Other Party Strategy)
            //Only When Upper Strategy can Shift to This Guy
            if (profile.right_strategy_payoff_ > fixed_strategy->the_other_party_max_pay_off_) {
                //Upper Guys can Shift to This Guy
                possible_to_be_updated_stored_strategies.clear();
                fixed_strategy->the_other_party_max_pay_off_ = profile.right_strategy_payoff_;
            }

            if (profile.left_strategy_payoff_ >= stored_strategy.the_other_party_max_pay_off_ &&
                profile.right_strategy_payoff_ >= fixed_strategy->the_other_party_max_pay_off_) {
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
        first_party, second_party, seats_num), has_swap_party_order_(false) {
    if (getFirst_party_()->GetSumVotes() > getSecond_party_()->GetSumVotes()) {
        Party *temp = getFirst_party_();
        setFirst_party_(getSecond_party_());
        setSecond_party_(temp);
        has_swap_party_order_ = true;
    }
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
    possible_to_be_nash_equilibrium_first_alpha_strategies_.clear();
    possible_to_be_nash_equilibrium_second_alpha_strategies_.clear();
}

SeatNumber AlphaBetaPruningSolverNaive::TraverseUsingPruning(vector<Strategy *> &beta_strategies,
                                                             vector<Strategy *> &alpha_strategies,
                                                             vector<Strategy *> &possible_nash_alpha_strategies) {
    // max of minimals
    SeatNumber max_alpha = -1;
    for (int i = 0; i < alpha_strategies.size(); i++) {
        Strategy *alpha_strategy = alpha_strategies[i];
        SeatNumber min_value = TraverseBetaStrategies(beta_strategies, alpha_strategy, max_alpha);
        if (min_value > max_alpha + DOUBLE_PRECISION) {
            for (vector<Strategy *>::iterator strategy_iterator = possible_nash_alpha_strategies.begin();
                 strategy_iterator != possible_nash_alpha_strategies.end(); strategy_iterator++) {
                Strategy *alpha_strategy = *strategy_iterator;
                alpha_strategy->possible_nash_equilibrium_.clear();
            }
            possible_nash_alpha_strategies.clear();
            max_alpha = min_value;
        }

        if (min_value > max_alpha + DOUBLE_PRECISION || abs(min_value - max_alpha) < DOUBLE_PRECISION) {
            possible_nash_alpha_strategies.push_back(alpha_strategy);
        }
    }
    return max_alpha;
}

SeatNumber AlphaBetaPruningSolverNaive::TraverseBetaStrategies(vector<Strategy *> &beta_strategies,
                                                               Strategy *alpha_strategy, SeatNumber &max_of_minimals) {
    vector<Strategy *> &possible_to_be_nash_equilibrium_beta_strategies = alpha_strategy->possible_nash_equilibrium_;
    SeatNumber min_of_beta_values = 100;
    for (int j = 0; j < beta_strategies.size(); j++) {
        Strategy *beta_strategy = beta_strategies[j];
        Profile profile = ComputePayOff(alpha_strategy, beta_strategy);
        SeatNumber alpha_value = profile.left_strategy_payoff_;

        if (alpha_value < min_of_beta_values - DOUBLE_PRECISION) {
            possible_to_be_nash_equilibrium_beta_strategies.clear();
            min_of_beta_values = alpha_value;
            if (min_of_beta_values < max_of_minimals - DOUBLE_PRECISION) {
                alpha_strategy->possible_nash_equilibrium_.clear();
                return min_of_beta_values;
            }
        }

        if (alpha_value < min_of_beta_values - DOUBLE_PRECISION ||
            abs(alpha_value - min_of_beta_values) < DOUBLE_PRECISION) {
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
//    cout << time(NULL) << "Finish Init" << endl;
    SeatNumber second_party_alpha_max1 = TraverseUsingPruning(first_party_strategies_, second_party_strategies_,
                                                              possible_to_be_nash_equilibrium_first_alpha_strategies_);
//    cout << time(NULL) << "Finish First" << endl;
    SeatNumber first_party_alpha_max2 = TraverseUsingPruning(second_party_strategies_, first_party_strategies_,
                                                             possible_to_be_nash_equilibrium_second_alpha_strategies_);
//    cout << time(NULL) << "Finish Second" << endl;
    vector<Strategy *> &first_party_possible_nash_strategies = possible_to_be_nash_equilibrium_second_alpha_strategies_;
    vector<Strategy *> &second_party_possible_nash_strategies = possible_to_be_nash_equilibrium_first_alpha_strategies_;

    InitSinglePartyMaximumPossibleNashStrategiesIdMap(first_party_possible_nash_strategies,
                                                      first_party_possible_nash_alpha_strategies_id_map_);
//    cout << time(NULL) << "Finish First Result Init" << endl;
    InitSinglePartyMaximumPossibleNashStrategiesIdMap(second_party_possible_nash_strategies,
                                                      second_party_possible_nash_alpha_strategies_id_map_);
//    cout << time(NULL) << "Finish Second Result Init" << endl;
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
            if (second_party_possible_nash_alpha_strategies_id_map_.find(*strategy_iterator) !=
                second_party_possible_nash_alpha_strategies_id_map_.end()) {
                size_t col_num = second_party_possible_nash_alpha_strategies_id_map_[*strategy_iterator];
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
            if (first_party_possible_nash_alpha_strategies_id_map_.find(*strategy_iterator) !=
                first_party_possible_nash_alpha_strategies_id_map_.end()) {
                size_t row_num = first_party_possible_nash_alpha_strategies_id_map_[*strategy_iterator];
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
    first_alpha_possible_nash_bitmap_ = new unsigned char[char_size];
    memset(first_alpha_possible_nash_bitmap_, 0, char_size);

}

void AlphaBetaPruningSolverWithBits::PrintNashEquilibrium() {
    //second_party_strategies_ as Row
    std::ios_base::sync_with_stdio(false);
    cout << setiosflags(ios::fixed) << setprecision(2);

//    size_t start_time = time(NULL);
    second_party_payoff_ = TraverseUsingPruning(first_party_strategies_, second_party_strategies_, true);
//    size_t end_time = time(NULL);
//    cout << "First Round:" << (end_time - start_time) << endl;

    first_party_payoff_ = getSeats_num_() - second_party_payoff_;
    size_t first_party_size = first_party_strategies_.size();
    size_t second_party_size = second_party_strategies_.size();
    size_t all_profile_size = first_party_size * second_party_size;
    unsigned char *&second_party_as_row_profiles = first_alpha_possible_nash_bitmap_;

    if (getSeats_num_() == 1) {
        for (size_t second_party_bit_index = 0; second_party_bit_index < all_profile_size; second_party_bit_index++) {
            size_t second_party_as_row_char_index = second_party_bit_index / BYTE_SIZE;
            size_t second_party_as_row_index_in_eight_bits = second_party_bit_index % BYTE_SIZE;
            unsigned char tmp_char = 0x01;
            unsigned char second_party_with_one_bit = tmp_char &
                                                      (second_party_as_row_profiles[second_party_as_row_char_index] >>
                                                       (BYTE_SIZE - 1 - second_party_as_row_index_in_eight_bits));
            size_t second_party_row_num = second_party_bit_index / first_party_size;
            size_t second_party_col_num = second_party_bit_index % first_party_size;
            if (second_party_with_one_bit == tmp_char) {
                //Print
                Strategy *first_party_strategy = first_party_strategies_[second_party_col_num];
                Strategy *second_party_strategy = second_party_strategies_[second_party_row_num];
                stringstream string_builder;
                if (has_swap_party_order_ == false) {
                    string_builder << "({" << first_party_strategy->ToString() << "," <<
                    second_party_strategy->ToString() << "})" <<
                    " ";
                    string_builder << "payoff " << "(" << first_party_payoff_ << "," <<
                    second_party_payoff_ << ")";
                    cout << string_builder.str() << endl;
                }
                else {
                    string_builder << "({" << second_party_strategy->ToString() << "," <<
                    first_party_strategy->ToString() << "})" <<
                    " ";
                    string_builder << "payoff " << "(" << second_party_payoff_ << "," <<
                    first_party_payoff_ << ")";
                    cout << string_builder.str() << endl;
                }
            }
        }
    }
    else if (getSeats_num_() > 1) {
        TraverseUsingPruning(second_party_strategies_, first_party_strategies_, false);

    }
//    cout << "Finished:" << time(NULL) - start_time << endl;
}

SeatNumber AlphaBetaPruningSolverWithBits::TraverseUsingPruning(vector<Strategy *> &beta_strategies,
                                                                vector<Strategy *> &alpha_strategies,
                                                                bool is_first_in) {
    // max of minimals
    SeatNumber max_alpha = -1;
    if (!is_first_in)
        max_alpha = first_party_payoff_;
    for (int row_num = 0; row_num < alpha_strategies.size(); row_num++) {
        Strategy *alpha_strategy = alpha_strategies[row_num];
        SeatNumber min_value = TraverseBetaStrategies(beta_strategies, alpha_strategy,
                                                      max_alpha, row_num, is_first_in);
        if (min_value > max_alpha + DOUBLE_PRECISION) {
            max_alpha = min_value;
        }
    }
    return max_alpha;
}

SeatNumber AlphaBetaPruningSolverWithBits::TraverseBetaStrategies(vector<Strategy *> &beta_strategies,
                                                                  Strategy *alpha_strategy,
                                                                  SeatNumber &max_of_minimals, int row_num,
                                                                  bool is_first_in) {
    vector<int> possible_to_be_nash_equilibrium_beta_strategies;
    SeatNumber min_of_beta_values = 100;
    for (int col_num = 0; col_num < beta_strategies.size(); col_num++) {
        Strategy *beta_strategy = beta_strategies[col_num];
        Profile profile = ComputePayOff(alpha_strategy, beta_strategy);

        SeatNumber alpha_value = profile.left_strategy_payoff_;

        if (alpha_value < min_of_beta_values - DOUBLE_PRECISION) {
            possible_to_be_nash_equilibrium_beta_strategies.clear();
            min_of_beta_values = alpha_value;
            if (min_of_beta_values < max_of_minimals - DOUBLE_PRECISION) {
                return min_of_beta_values;
            }
        }

        if (alpha_value < min_of_beta_values - DOUBLE_PRECISION ||
            abs(alpha_value - min_of_beta_values) < DOUBLE_PRECISION) {
            possible_to_be_nash_equilibrium_beta_strategies.push_back(col_num);
        }
    }

    size_t first_party_size = first_party_strategies_.size();

    if (is_first_in && min_of_beta_values > max_of_minimals + DOUBLE_PRECISION) {
        memset(first_alpha_possible_nash_bitmap_, 0, first_party_size * row_num);
    }

    for (vector<int>::iterator strategy_iterator = possible_to_be_nash_equilibrium_beta_strategies.begin();
         strategy_iterator != possible_to_be_nash_equilibrium_beta_strategies.end(); strategy_iterator++) {
        if (is_first_in) {
            size_t col_num = *strategy_iterator;
            size_t bit_index = row_num * first_party_size + col_num;
            size_t char_index = bit_index / BYTE_SIZE;
            int index_in_eight_bits_from_high_to_low = bit_index % BYTE_SIZE;
            unsigned char tmp_char = 0x01;
            tmp_char = tmp_char << (BYTE_SIZE - 1 - index_in_eight_bits_from_high_to_low);
            first_alpha_possible_nash_bitmap_[char_index] |= tmp_char;
        }

        else if (!is_first_in) {
            size_t col_num = *strategy_iterator;
            size_t bit_index = col_num * first_party_size + row_num;
            size_t char_index = bit_index / BYTE_SIZE;
            int index_in_eight_bits_from_high_to_low = bit_index % BYTE_SIZE;
            unsigned char tmp_char = 0x01;
            if ((first_alpha_possible_nash_bitmap_[char_index] >>
                 (BYTE_SIZE - 1 - index_in_eight_bits_from_high_to_low)) &
                tmp_char == tmp_char) {

                if (has_swap_party_order_ == false) {
                    Strategy *first_party_strategy = first_party_strategies_[row_num];
                    Strategy *second_party_strategy = second_party_strategies_[col_num];
                    stringstream string_builder;
                    string_builder << "({" << first_party_strategy->ToString() << "," <<
                    second_party_strategy->ToString() << "})" <<
                    " ";
                    string_builder << "payoff " << "(" << first_party_payoff_ << "," <<
                    second_party_payoff_ << ")";
                    cout << string_builder.str() << endl;
                }
                else {
                    Strategy *first_party_strategy = first_party_strategies_[row_num];
                    Strategy *second_party_strategy = second_party_strategies_[col_num];
                    stringstream string_builder;
                    string_builder << "({" << second_party_strategy->ToString() << "," <<
                    first_party_strategy->ToString() << "})" <<
                    " ";
                    string_builder << "payoff " << "(" << second_party_payoff_ << "," <<
                    first_party_payoff_ << ")";
                    cout << string_builder.str() << endl;
                }

            }
        }
    }
    return min_of_beta_values;
}

AlphaBetaPruningSolverWithBits::~AlphaBetaPruningSolverWithBits() {
    delete first_alpha_possible_nash_bitmap_;
}



