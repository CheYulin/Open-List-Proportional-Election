//
// Created by yche on 11/20/2015.
//

#include "open_list_solver.h"
using namespace election;

//Profile Related
Profile::Profile(Strategy *first_party_strategy, Strategy *second_party_strategy) : first_party_strategy_(
        first_party_strategy), second_party_strategy_(second_party_strategy) {

}

//Solver Related
Solver::Solver(Party *first_party, Party *second_party, int seats_num) :
        first_party_(first_party), second_party_(second_party), seats_num_(seats_num) {

}

Solver::~Solver() {
    delete first_party_;
    delete second_party_;
}

Profile election::Solver::ComputePayOff(Strategy *first_party_strategy,
                                        Strategy *second_party_strategy) {
    int sum_votes = first_party_->GetSumVotes() + second_party_->GetSumVotes();
    int remaining_seats = seats_num_;
    int quota = sum_votes / seats_num_;
    priority_queue<Group, vector<Group>, GroupComparePartition> first_party_groups_info = first_party_strategy->groups_info_;
    priority_queue<Group, vector<Group>, GroupComparePartition> second_party_groups_info = second_party_strategy->groups_info_;
    Profile profile(first_party_strategy, second_party_strategy);

    //First Round with Quota
    while (!first_party_groups_info.empty() && first_party_groups_info.top().group_vote_count_ >= quota) {
        first_party_groups_info.pop();
        profile.first_party_payoff_ += 1;
    }
    while (!second_party_groups_info.empty() && second_party_groups_info.top().group_vote_count_ >= quota) {
        second_party_groups_info.pop();
        profile.second_party_payoff_ += 1;
    }

    //Next Several Rounds
    while (remaining_seats > 0) {
        int max_vote_first_party = first_party_groups_info.empty() ? 0 : first_party_groups_info.top().group_vote_count_;
        int max_vote_second_party = second_party_groups_info.empty() ? 0
                                                                     : second_party_groups_info.top().group_vote_count_;
        int max_vote = max(max_vote_first_party, max_vote_second_party);

        int first_party_pop_num = 0;
        int second_party_pop_num = 0;
        while (!first_party_groups_info.empty() && first_party_groups_info.top().group_vote_count_ == max_vote) {
            first_party_groups_info.pop();
            first_party_pop_num++;
        }
        while (!second_party_groups_info.empty() && second_party_groups_info.top().group_vote_count_ == max_vote) {
            second_party_groups_info.pop();
            second_party_pop_num++;
        }
        int sum_pop_num = first_party_pop_num + second_party_pop_num;
        if (sum_pop_num > remaining_seats) {
            profile.first_party_payoff_ += remaining_seats * first_party_pop_num / sum_pop_num;
            profile.second_party_payoff_ += remaining_seats * second_party_pop_num / sum_pop_num;
        }
        else {
            profile.first_party_payoff_ += first_party_pop_num;
            profile.second_party_payoff_ += second_party_pop_num;
        }
    }

    return profile;
}


