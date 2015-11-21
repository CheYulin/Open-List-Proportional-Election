//
// Created by cheyulin on 11/19/15.
//
#ifndef SRC_OPEN_LIST_DATA_H
#define SRC_OPEN_LIST_DATA_H

#endif //SRC_OPEN_LIST_DATA_H

#include "open_list_includes.h"

namespace election {
    typedef int CandidateId;
    typedef int CandidateVoteCount;
    typedef string CandidateName;
    typedef double StrategyPayOff;

    struct CandidateInfo {
        CandidateName candidate_name_;
        CandidateVoteCount candidate_vote_count_;

        CandidateInfo();

        CandidateInfo(CandidateName candidate_name, CandidateVoteCount candidate_vote_count);
    };

    struct Group {
        vector<CandidateId> candidates_;
        int group_vote_count_;

        string GetCandidatesAsString() const;
    };

    struct GroupCandidatesCompare {
        bool operator()(const Group &left_group, const Group &right_group) const;
    };

    struct GroupVoteCompare {
        bool operator()(const Group *left_group, const Group *right_group);
    };

    typedef set<Group, GroupCandidatesCompare> CompareCandidatesGroupSet;
    typedef priority_queue<const Group *, vector<const Group *>, GroupVoteCompare> CompareVoteGroupPriorityQueue;

    struct Strategy {
        CompareVoteGroupPriorityQueue groups_info_;
        vector<Strategy> possible_nash_equilibrium_;
        StrategyPayOff max_pay_off_;

        Strategy();
    };

    typedef vector<Strategy> SameSizeStrategies;

    typedef set<CandidateId> GroupInPartition;
    typedef vector<GroupInPartition> Partition;
    typedef vector<Partition> SameSizePartitions;
    typedef vector<SameSizePartitions> DifferentSizePartitions;

    class Party {
    private:
        int sum_votes_;
        map<CandidateId, CandidateInfo> candidates_info_;
        vector<CompareCandidatesGroupSet> groups_info_with_different_size_;
        vector<SameSizeStrategies> strategies_with_different_size_;

        void InitGroupsAlternativesInfo();

        //Generate Partitions with Different Sizes and Turn Last time Result to PriorityQueue
        void AddNewCandidateIdToMakeEqualSizePartition(const Partition &former_partition,
                                                       const CandidateId &candidate_id,
                                                       SameSizePartitions *latter_same_size_partition_i);

        void AddNewCandidateIdToMakeSizePlusOnePartition(const Partition &former_partition,
                                                         const CandidateId &candidate_id,
                                                         SameSizePartitions *latter_same_size_partition_i_plus_one);

        void TransformPartitionIntoPriorityQueueGetStrategies(DifferentSizePartitions *different_size_partitions);

    public:
        Party(vector<CandidateInfo> candidates_info, int seats_num);

        int GetSumVotes();

        const Group *GetExactGroupPointer(const Group &to_be_found_group);

        void InitStrategies();

        //Auto Generated
        const vector<CompareCandidatesGroupSet> &getGroups_info_with_different_size_() const {
            return groups_info_with_different_size_;
        }

        const map<CandidateId, CandidateInfo> &getCandidates_info_() const {
            return candidates_info_;
        }

        const vector<SameSizeStrategies> &getStrategies_with_different_size_() const {
            return strategies_with_different_size_;
        }
    };

}
