//
// Created by cheyulin on 11/23/15.
//

#ifndef SRC_OPEN_LIST_PARTY_BASIC_INFO_H
#define SRC_OPEN_LIST_PARTY_BASIC_INFO_H

#endif //SRC_OPEN_LIST_PARTY_BASIC_INFO_H

#include "open_list_includes.h"

namespace election{
    typedef int CandidateId;
    typedef int CandidateVoteCount;
    typedef string CandidateName;
    typedef double StrategyPayOff;

    class Party;

    struct CandidateInfo {
        CandidateName candidate_name_;
        CandidateVoteCount candidate_vote_count_;

        CandidateInfo();

        CandidateInfo(CandidateName candidate_name, CandidateVoteCount candidate_vote_count);
    };

    struct CandidateListInfo {
        vector<CandidateId>* candidates_;
        int group_vote_count_;

        CandidateListInfo();
        CandidateListInfo(const CandidateListInfo& candidate_list_info);
        string GetCandidatesAsString() const;
    };

    struct GroupCandidatesCompare {
        bool operator()(const CandidateListInfo *left_group, const CandidateListInfo *right_group) const;
    };

    struct GroupVoteCompare {
        bool operator()(const CandidateListInfo *left_group, const CandidateListInfo *right_group);
    };

    typedef set<const CandidateListInfo *, GroupCandidatesCompare> CompareNameCandidatesListInfoSet;
    typedef priority_queue<const CandidateListInfo *, vector<const CandidateListInfo *>, GroupVoteCompare> CompareVoteCandidateListPriorityQueue;


}