//
// Created by cheyulin on 11/19/15.
//

#include "open_list_party.h"

Party::Party(vector<CandidateInfo> candidates_info, int seats_num) {
    int candidate_id = 1;
    for(CandidateInfo candidate_info : candidates_info){
        candidates_info_.insert(make_pair(candidate_id,candidate_info));
    }
    InitGroupsAlternativesInfo();
}

int Party::GetSumVotes() {
    if (sum_votes_ == 0) {
        int count = 0;
        for (auto candidate_info : candidates_info_) {
            count += candidate_info.first;
        }
    }
    else
        return sum_votes_;
}

void Party::InitGroupsAlternativesInfo() {
    int party_size = candidates_info_.size();
    groups_info_with_different_size_ = vector<vector<Group>>(party_size);
    vector<Group> first_vector = vector<Group>();

    //initialize group only with 1 person
    for (auto candidate_info : candidates_info_) {
        Group group = Group();
        group.candidates_.push_back(candidate_info.first);
        group.group_vote_count_ = candidate_info.second.candidate_vote_count_;
        first_vector.push_back(group);
    }
    groups_info_with_different_size_.push_back(first_vector);

    //initialize latter groups with size increasing
    vector<Group> former_groups_with_same_size = first_vector;
    vector<Group> latter_groups_with_same_size = vector<Group>();
    for (int i = 1; i < party_size; i++) {
        for (Group former_data: former_groups_with_same_size) {
            int last_value = former_data.candidates_[former_data.candidates_.size() - 1];
            int max_value = first_vector[first_vector.size() - 1].candidates_[0];
            for (int j = last_value + 1; j <= max_value; j++) {
                Group new_data = former_data;
                new_data.candidates_.push_back(j);
                new_data.group_vote_count_ += first_vector[j-1].group_vote_count_;
                latter_groups_with_same_size.push_back(new_data);
            }
        }
        groups_info_with_different_size_.push_back(latter_groups_with_same_size);
        former_groups_with_same_size = latter_groups_with_same_size;
        latter_groups_with_same_size = vector<Group>();
    }


}


