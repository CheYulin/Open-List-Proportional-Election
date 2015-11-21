//
// Created by cheyulin on 11/19/15.
//

#include "open_list_party.h"

using namespace election;

//Candidate Related
CandidateInfo::CandidateInfo() {

}

CandidateInfo::CandidateInfo(CandidateName candidate_name, CandidateVoteCount candidate_vote_count) : candidate_name_(
        candidate_name), candidate_vote_count_(candidate_vote_count) {

}

//Group Related
bool GroupComparePartition::operator()(const Group &left_group, const Group &right_group) const {
    int left_collection_size = left_group.candidates_.size();
    int right_collection_size = right_group.candidates_.size();
    int max_size = left_collection_size < right_collection_size ? left_collection_size : right_collection_size;
    for (int i = 0; i < max_size; i++) {
        if (left_group.candidates_[i] < right_group.candidates_[i]) {
            return true;
        }
        else if (left_group.candidates_[i] > right_group.candidates_[i]) {
            return false;
        }
    }
    if (max_size < right_collection_size) {
        return true;
    }
    else
        return false;
}

bool GroupCompareVote::operator()(const Group &left_group, const Group &right_group) const {
    return left_group.group_vote_count_ < right_group.group_vote_count_;
}

//Party Related
Party::Party(vector<CandidateInfo> candidates_info, int seats_num) {
    int candidate_id = 1;
    for (CandidateInfo candidate_info : candidates_info) {
        candidates_info_.insert(make_pair(candidate_id, candidate_info));
        candidate_id++;
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

    //First Initialize First Layer Groups with Size One
    groups_info_with_different_size_ = vector<ComparePartitionGroupSet>();
    ComparePartitionGroupSet first_groups_with_size_one = ComparePartitionGroupSet();
    vector<Group> first_groups_with_size_one_info = vector<Group>();
    for (auto pair : candidates_info_) {
        Group data = Group();
        data.candidates_.push_back(pair.first);
        data.group_vote_count_ = pair.second.candidate_vote_count_;
        first_groups_with_size_one.insert(data);
        first_groups_with_size_one_info.push_back(data);
    }
    groups_info_with_different_size_.push_back(first_groups_with_size_one);

    //Next Deal with Groups with Size more than One
    ComparePartitionGroupSet former_groups_with_same_size = first_groups_with_size_one;
    ComparePartitionGroupSet latter_groups_with_same_size = ComparePartitionGroupSet();
    int max_value = first_groups_with_size_one_info[first_groups_with_size_one_info.size() - 1].candidates_[0];
    for (int i = 1; i < party_size; i++) {
        for (Group former_data: former_groups_with_same_size) {
            int last_value = former_data.candidates_[former_data.candidates_.size() - 1];
            for (int j = last_value + 1; j <= max_value; j++) {
                Group new_data = former_data;
                new_data.candidates_.push_back(j);
                new_data.group_vote_count_ += first_groups_with_size_one_info[j - 1].group_vote_count_;
                latter_groups_with_same_size.insert(new_data);
            }
        }
        groups_info_with_different_size_.push_back(latter_groups_with_same_size);
        former_groups_with_same_size = latter_groups_with_same_size;
        latter_groups_with_same_size = ComparePartitionGroupSet();
    }

}



