//
// Created by yche on 11/20/2015.
//
#include <iostream>
#include "open_list_party.h"

using namespace std;

struct Group {
    vector<int> collection;
    int vote;
};

int main() {
    map<int, int> my_map;
    my_map.insert(make_pair(1, 2));
    my_map.insert(make_pair(2, 3));
    my_map.insert(make_pair(3, 2));
    my_map.insert(make_pair(4, 4));
    my_map.insert(make_pair(5,7));
    int map_size = my_map.size();

    vector<vector<Group>> my_vector_vector = vector<vector<Group>>(map_size);
    vector<Group> first_vector = vector<Group>();
    for (auto pair : my_map) {
        Group data = Group();
        data.collection.push_back(pair.first);
        data.vote = pair.second;
        first_vector.push_back(data);
//        first_vector.push_back(data);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      );
    }
    my_vector_vector.push_back(first_vector);

    vector<Group> former_vector = first_vector;
    vector<Group> latter_vector = vector<Group>();
    for (int i = 1; i < map_size; i++) {
        for (Group former_data: former_vector) {
            int last_value = former_data.collection[former_data.collection.size() - 1];
            int max_value = first_vector[first_vector.size() - 1].collection[0];
            for (int j = last_value + 1; j <= max_value; j++) {
                Group new_data = former_data;
                new_data.collection.push_back(j);
                new_data.vote+= first_vector[j-1].vote;
                latter_vector.push_back(new_data);
            }
        }
        my_vector_vector.push_back(latter_vector);
        former_vector = latter_vector;
        latter_vector = vector<Group>();
    }

    for(vector<Group> vectors: my_vector_vector){
        for(Group data: vectors){
            cout << "(";
            for(int integer : data.collection){
                cout<<integer;
            }
            cout<<"," << data.vote<< ")"<< "  ";
        }
        cout<< endl;
    }
}

