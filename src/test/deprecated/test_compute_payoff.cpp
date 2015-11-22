//
// Created by cheyulin on 11/19/15.
//
#include "../../open_list_includes.h"

struct Pair{
    Pair(int key, int value): key(key), value(value){};
    int key;
    int value;
};

struct CmpPair {
    bool operator() (Pair a, Pair b){
            return a.key < b.key;
    };

};

int main(){
    multiset<Pair, CmpPair> my_set;
    my_set.insert(Pair(1,2));
    my_set.insert(Pair(2,3));
    my_set.insert(Pair(3,4));
    my_set.insert(Pair(3,5));
    for(multiset<Pair, CmpPair>::iterator iter = my_set.begin(); iter != my_set.end(); iter++){
        cout << iter->key << "," << iter->value << endl;
    }
    cout<< endl;

    priority_queue<Pair, vector<Pair>, CmpPair> priority_queue1;
    priority_queue1.push(Pair(1,2));
    priority_queue1.push(Pair(2,3));
    priority_queue1.push(Pair(3,4));
    priority_queue1.push(Pair(3,5));

    int max = priority_queue1.top().key;
    while(!priority_queue1.empty() && priority_queue1.top().key== max){
        Pair pair1 = priority_queue1.top();
        priority_queue1.pop();
        cout << pair1.key <<","<< pair1.value << endl;
    }
}