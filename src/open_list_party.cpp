//
// Created by cheyulin on 11/19/15.
//

#include "open_list_party.h"

using namespace election;

//Strategy Related
Strategy::Strategy(Party *party) : the_other_party_max_pay_off_(-1), party_(party) {
}

string Strategy::ToString() {
    CompareVoteCandidateListPriorityQueue priority_queue = groups_combination_info_;
    CompareNameCandidatesListInfoSet compare_candidate_group_set;
    stringstream string_builder;
    string_builder << "{";
    while (!priority_queue.empty()) {
        compare_candidate_group_set.insert(priority_queue.top());
        priority_queue.pop();
    }
    for (const CandidateListInfo *group : compare_candidate_group_set) {
        string_builder << "{";
        for (int i = 0; i < group->candidates_->size(); i++) {
            string_builder << party_->getCandidates_info_().at((*(group->candidates_))[i]).candidate_name_;
            if (i != group->candidates_->size() - 1) {
                string_builder << ",";
            }
        }
        string_builder << "}";
    }
    string_builder << "}";

    return string_builder.str();
}

//Party Related
Party::Party(vector<CandidateInfo> candidates_info, int seats_num) :sum_votes_(0){
    int candidate_id = 1;
    for (CandidateInfo candidate_info : candidates_info) {
        candidates_info_.insert(make_pair(candidate_id, candidate_info));
        candidate_id++;
    }
    InitGroupsAlternativesInfo();
}

Party::~Party() {
    for (CompareNameCandidatesListInfoSet &same_size_candidate_list : groups_info_with_different_size_) {
        for (CompareNameCandidatesListInfoSet::iterator my_iterator = same_size_candidate_list.begin();
             my_iterator != same_size_candidate_list.end(); my_iterator++) {
            delete *my_iterator;
        }
    }
}

int Party::GetSumVotes() {
    if (sum_votes_ == 0) {
        for (auto iter = candidates_info_.begin(); iter!= candidates_info_.end(); iter++) {
            sum_votes_ +=iter->second.candidate_vote_count_;
        }

    }
    return sum_votes_;
}

void Party::InitGroupsAlternativesInfo() {
    int party_size = candidates_info_.size();

    //First Initialize First Layer Groups with Size One
    groups_info_with_different_size_ = vector<CompareNameCandidatesListInfoSet>();
    CompareNameCandidatesListInfoSet first_groups_with_size_one = CompareNameCandidatesListInfoSet();
    vector<CandidateListInfo*> first_groups_with_size_one_info = vector<CandidateListInfo*>();
    for (auto pair : candidates_info_) {
        CandidateListInfo *candidate_list_info = new CandidateListInfo();
        candidate_list_info->candidates_->push_back(pair.first);
        candidate_list_info->group_vote_count_ = pair.second.candidate_vote_count_;
        first_groups_with_size_one.insert(candidate_list_info);
        first_groups_with_size_one_info.push_back(candidate_list_info);
    }
    groups_info_with_different_size_.push_back(first_groups_with_size_one);

    //Next Deal with Groups with Size more than One
    CompareNameCandidatesListInfoSet former_groups_with_same_size = first_groups_with_size_one;
    CompareNameCandidatesListInfoSet latter_groups_with_same_size = CompareNameCandidatesListInfoSet();
    int max_value = (*first_groups_with_size_one_info[first_groups_with_size_one_info.size() - 1]->candidates_)[0];
    for (int i = 1; i < party_size; i++) {
        for (const CandidateListInfo *former_candidate_list_info: former_groups_with_same_size) {
            int last_value = (*former_candidate_list_info->candidates_)[
                    former_candidate_list_info->candidates_->size() - 1];
            for (int j = last_value + 1; j <= max_value; j++) {
                CandidateListInfo *new_candidate_list_info = new CandidateListInfo(*former_candidate_list_info);
                new_candidate_list_info->candidates_->push_back(j);
                new_candidate_list_info->group_vote_count_ += first_groups_with_size_one_info[j - 1]->group_vote_count_;
                latter_groups_with_same_size.insert(new_candidate_list_info);
            }
        }
        groups_info_with_different_size_.push_back(latter_groups_with_same_size);
        former_groups_with_same_size = latter_groups_with_same_size;
        latter_groups_with_same_size = CompareNameCandidatesListInfoSet();
    }

}

const CandidateListInfo *Party::GetExactGroupPointer(const CandidateListInfo *to_be_found_group) {
    int size = to_be_found_group->candidates_->size();
    CompareNameCandidatesListInfoSet::iterator iter = groups_info_with_different_size_[size - 1].find(to_be_found_group);

    return *iter;
}

//Party Generate Different Size Partition Related
void Party::AddNewCandidateIdToMakeEqualSizePartition(const Partition &former_partition,
                                                      const CandidateId &candidate_id,
                                                      SameSizePartitions *latter_same_size_partition_i) {
    for (int i = 0; i < former_partition.size(); i++) {
        Partition new_partition = former_partition;
        new_partition[i].insert(candidate_id);
        latter_same_size_partition_i->push_back(new_partition);
    }
}

void Party::AddNewCandidateIdToMakeSizePlusOnePartition(const Partition &former_partition,
                                                        const CandidateId &candidate_id,
                                                        SameSizePartitions *latter_same_size_partition_i_plus_one) {
    Partition new_partition = former_partition;
    GroupInPartition new_group;
    new_group.insert(candidate_id);
    new_partition.push_back(new_group);
    latter_same_size_partition_i_plus_one->push_back(new_partition);
}

void Party::TransformPartitionIntoPriorityQueueGetStrategies(DifferentSizePartitions *different_size_partitions) {
    for (SameSizePartitions same_size_partitions: *different_size_partitions) {
        SameSizeStrategies same_size_strategies;
        Strategy *strategy;
        for (Partition partition: same_size_partitions) {
            strategy = new Strategy(this);
            CandidateListInfo *to_be_find_group;
            for (GroupInPartition group_in_partition : partition) {
                to_be_find_group = new CandidateListInfo();
                for (CandidateId candidate_id: group_in_partition) {
                    to_be_find_group->candidates_->push_back(candidate_id);
                }
                strategy->groups_combination_info_.push(GetExactGroupPointer(to_be_find_group));
                delete to_be_find_group;
            }
            same_size_strategies.push_back(*strategy);
            delete strategy;
        }
        strategies_with_different_size_.push_back(same_size_strategies);
    }
}

void Party::InitStrategies() {
    CandidateId candidate_id = 1;

    GroupInPartition group_in_partition;
    group_in_partition.insert(candidate_id);

    Partition partition;
    partition.push_back(group_in_partition);

    SameSizePartitions same_size_partitions;
    same_size_partitions.push_back(partition);

    DifferentSizePartitions *first_different_size_partitions = new DifferentSizePartitions();
    first_different_size_partitions->push_back(same_size_partitions);

    DifferentSizePartitions *former_different_size_partitions = first_different_size_partitions;
    DifferentSizePartitions *latter_different_size_partitions = nullptr;

    //Iteration between Different Max Party Size
    int party_size = candidates_info_.size();
    for (int latter_party_size = 2; latter_party_size <= party_size; latter_party_size++) {
        latter_different_size_partitions = new DifferentSizePartitions(latter_party_size);
        CandidateId latter_candidate_id = latter_party_size;
        for (int former_party_partition_possible_size = 1;
             former_party_partition_possible_size <= latter_party_size - 1; former_party_partition_possible_size++) {
            SameSizePartitions *former_same_size_partitions = &(*former_different_size_partitions)
            [former_party_partition_possible_size - 1];
            SameSizePartitions *latter_same_size_partitions_i = &(*latter_different_size_partitions)
            [former_party_partition_possible_size - 1];
            SameSizePartitions *latter_same_size_partitions_i_plus_one = &(*latter_different_size_partitions)
            [former_party_partition_possible_size];
            for (Partition former_partition: *former_same_size_partitions) {
                AddNewCandidateIdToMakeEqualSizePartition(former_partition, latter_candidate_id,
                                                          latter_same_size_partitions_i);
                AddNewCandidateIdToMakeSizePlusOnePartition(former_partition, latter_candidate_id,
                                                            latter_same_size_partitions_i_plus_one);
            }
        }
        delete former_different_size_partitions;
        former_different_size_partitions = latter_different_size_partitions;
    }

    //Put last Result Into Priority Queue
    TransformPartitionIntoPriorityQueueGetStrategies(latter_different_size_partitions);
    delete latter_different_size_partitions;
}








