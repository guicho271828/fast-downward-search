/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_BANDIT_H
#define OPEN_LISTS_BANDIT_H
#include <limits>
// #include <deque>
#include <random>
#include <unordered_map>
#include <map>
#include <iostream>

using namespace std;


template<class Reward, class Entry>
class Lever {
    int play = 0;
    Reward reward = numeric_limits<Reward>::min();
public:
    // Lever() : play(0),reward(numeric_limits<Reward>::min()){};
    // ~Lever(){};
    int get_play(){return play;};
    Reward mean_reward(){
        if (play==0){
            return numeric_limits<Reward>::infinity();
        }else{
            return reward/play;
        }
    };
    void do_reward(Reward r){ play++; reward += r;};
    virtual Entry pull() = 0;
    virtual void push(Entry e) = 0;
};


template<class Reward, class Entry, template<typename,typename> class LT>
class Bandit {
    typedef LT<Reward,Entry> L;
    int play = 0;
    L* last_selected = nullptr;
public:
    map<int,L> levers;
    // unordered_map<int,L> levers;
    Bandit(){};
    ~Bandit(){};
    int get_play(){return play;};
    virtual L* select() final {
        last_selected = do_select();
        return last_selected;
    } ;
    void do_reward(Reward r){
        play++;
        assert(last_selected);
        last_selected->do_reward(r);
    };
    L& operator[](int i){return levers[i];};
    virtual L* do_select(){return nullptr;};
    virtual void dump(){};
};


template<class Reward, class Entry, template<typename,typename> class LT>
class UCB : public Bandit<Reward,Entry,LT> {
    typedef LT<Reward,Entry> L;
    L* do_select(){
        assert(!this->levers.empty()); 
        Reward max = -1 * numeric_limits<double>::infinity();
        L* best = nullptr;
        for (auto lever : this->levers){
            Reward s = score(lever.second);
            if (max < s){
                max = s;
                best = &(lever.second);
            }
        }
        assert(best);
        this->dump();
        return best;
    };
public:
    UCB(){};
    ~UCB(){};
    Reward score(L& lever){
        if (this->get_play()==0 || lever.get_play()==0){
            return numeric_limits<Reward>::infinity();
        }else{
            return
                lever.mean_reward() +
                sqrt(2 * log(this->get_play()) / lever.get_play());
        }
    };
    void dump() {
        vector<Reward> scores(this->levers.size());
        Reward max = -1 * numeric_limits<double>::infinity();
        int i = 0;
        int best_index = 0;
        cout << "[" ;
        auto it = this->levers.rbegin();
        while (it!=this->levers.rend()){
            Reward s = this->score(it->second);
            if (it->second.empty()){
                cout << "(" << s << ")";
            }else{
                cout << " " << s << " ";
                if (max < s){
                    max = s;
                    best_index = i;
                }
            }
            i++;
            it++;
        }
        cout << "] best: " << best_index << endl ;
    };
};


// template<class LT, class Reward, class Entry>
// class EpsilonDecreasing : public Bandit<LT,Reward,Entry> {
//     typedef Lever<Reward,Entry> L;
//     mt19937 gen = mt19937(1);
//     double initial_epsilon;
//     bool should_choose_best(){
//         return initial_epsilon/this->get_play() < generate_canonical<double,10>(gen);
//     }
//     L* do_select(){
//         assert(!this->levers.empty());
//         if(should_choose_best()){
//             Reward max = numeric_limits<Reward>::min();
//             L* best = nullptr;
//             for (auto lever : this->levers){
//                 Reward score = lever->mean_reward();
//                 if (max < score){
//                     max = score;
//                     best = &lever;
//                 }
//             }
// assert(best);
//             return best;
//         }else{
//             uniform_int_distribution<> dis(0,this->levers.size()-1);
//             return &(this->levers[dis(gen)]);
//         }
//     }
// public:
//     EpsilonDecreasing(){};
//     ~EpsilonDecreasing(){};
// };

#include "bandit.cc"

#endif
