//
// Created by cheyulin on 11/19/15.
//
#ifndef SRC_OPEN_LIST_DATA_H
#define SRC_OPEN_LIST_DATA_H

#endif //SRC_OPEN_LIST_DATA_H

#include "open_list_party_basic_info.h"
using namespace election;

namespace election {
    struct Strategy {
        CompareVoteCandidateListPriorityQueue groups_combination_info_;
        vector<Strategy *> possible_nash_equilibrium_;
        StrategyPayOff the_other_party_max_pay_off_;
        Party *party_;

        Strategy(Party *party);

        string ToString();
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
        vector<CompareNameCandidatesListInfoSet> groups_info_with_different_size_;
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

        ~Party();

        int GetSumVotes();

        const CandidateListInfo *GetExactGroupPointer(const CandidateListInfo *to_be_found_group);

        void InitStrategies();

        //Auto Generated
        const vector<CompareNameCandidatesListInfoSet> &getGroups_info_with_different_size_() const {
            return groups_info_with_different_size_;
        }

        const map<CandidateId, CandidateInfo> &getCandidates_info_() const {
            return candidates_info_;
        }

        vector<SameSizeStrategies> &getStrategies_with_different_size_() {
            return strategies_with_different_size_;
        }
    };

}
