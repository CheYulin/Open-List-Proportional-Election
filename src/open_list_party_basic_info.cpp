//
// Created by cheyulin on 11/23/15.
//

#include "open_list_party_basic_info.h"

using namespace election;

//Candidate Related
CandidateInfo::CandidateInfo() {
}

CandidateInfo::CandidateInfo(CandidateName candidate_name, CandidateVoteCount candidate_vote_count) : candidate_name_(
        candidate_name), candidate_vote_count_(candidate_vote_count) {
}


//CandidateListInfo Related
string CandidateListInfo::GetCandidatesAsString() const {
    stringstream string_builder;
    for (vector<CandidateId>::iterator candidate_id_iterator = (candidates_)->begin();
         candidate_id_iterator != candidates_->end(); candidate_id_iterator++) {
        CandidateId candidate_id = *candidate_id_iterator;
        string_builder << candidate_id;
    }
    return string_builder.str();
}

CandidateListInfo::CandidateListInfo() : first_round_seat_num_(-1), has_candidates_in_list_(true) {
    candidates_ = new vector<CandidateId>();
}

CandidateListInfo::CandidateListInfo(int remaining_vote_num) : group_vote_count_(remaining_vote_num) {

}

CandidateListInfo::CandidateListInfo(const CandidateListInfo &candidate_list_info) : group_vote_count_(
        candidate_list_info.group_vote_count_), first_round_seat_num_(-1), has_candidates_in_list_(true) {
    candidates_ = new vector<CandidateId>();

    for(vector<CandidateId>::iterator candidate_id_iterator = candidate_list_info.candidates_->begin(); candidate_id_iterator != candidate_list_info.candidates_->end();candidate_id_iterator++){
        CandidateId candidate_id = * candidate_id_iterator;
        candidates_->push_back(candidate_id);
    }
}

bool GroupCandidatesCompare::operator()(const CandidateListInfo *left_group,
                                        const CandidateListInfo *right_group) const {
    int left_collection_size = left_group->candidates_->size();
    int right_collection_size = right_group->candidates_->size();
    int max_size = left_collection_size < right_collection_size ? left_collection_size : right_collection_size;
    for (int i = 0; i < max_size; i++) {
        if ((*left_group->candidates_)[i] < (*right_group->candidates_)[i]) {
            return true;
        }
        else if ((*left_group->candidates_)[i] > (*right_group->candidates_)[i]) {
            return false;
        }
    }
    return max_size < right_collection_size;
}

bool GroupVoteCompare::operator()(const CandidateListInfo *left_group, const CandidateListInfo *right_group) {
    return left_group->group_vote_count_ < right_group->group_vote_count_;
}


