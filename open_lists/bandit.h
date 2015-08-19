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
    // depth-first
    map<int,L,greater<int>> levers;
    // unordered_map<int,L> levers;
    Bandit(){};
    ~Bandit(){};
    int get_play(){return play;};
    virtual L* select() final {
        assert(!levers.empty()); 
        last_selected = do_select();
        assert(last_selected);
        return last_selected;
    } ;
    void do_reward(Reward r){
        play++;
        assert(last_selected);
        last_selected->do_reward(r);
    };
    L& operator[](int i){return levers[i];};
    virtual L* do_select(){return nullptr;};
    L* select_max_score(){
        Reward max = -1 * numeric_limits<double>::infinity();
        L* best = nullptr;
        for (auto lever : this->levers){
            Reward s = score(lever.second);
            if (max < s){
                max = s;
                best = &(lever.second);
            }
        }
        return best;
    }
    virtual Reward score(L& lever) = 0;
    virtual void dump(){};
};


template<class Reward, class Entry, template<typename,typename> class LT>
class UCB : public Bandit<Reward,Entry,LT> {
    typedef LT<Reward,Entry> L;
    const Reward k;
public:
    UCB(Reward k) : k(k){};
    ~UCB(){};
    L* do_select(){return this->select_max_score();};
    Reward score(L& lever){
        if (this->get_play()==0 || lever.get_play()==0){
            return numeric_limits<Reward>::infinity();
        }else{
            return
                lever.mean_reward() +
                sqrt(k * log(this->get_play()) / lever.get_play());
        }
    };
    void dump() {
        vector<Reward> scores(this->levers.size());
        Reward max = -1 * numeric_limits<double>::infinity();
        int i = 0;
        int best_index = 0;
        auto it = this->levers.rbegin();
        while (it!=this->levers.rend()){
            Reward s = this->score(it->second);
            cout << " " << s << " ";
            if (max < s){
                max = s;
                best_index = i;
            }
            i++;
            it++;
            scores.push_back(s);
        }
        cout << scores << " best: " << best_index << endl ;
    };
};

template<class Reward, class Entry, template<typename,typename> class LT>
class EpsilonBandit : public Bandit<Reward,Entry,LT> {
    typedef Lever<Reward,Entry> L;
protected:
    mt19937 gen = mt19937(1);
    double initial_epsilon;
    virtual bool should_choose_best() = 0;
public:
    EpsilonBandit(double initial_epsilon) : initial_epsilon(initial_epsilon){};
    ~EpsilonBandit(){};
    L* do_select(){
        if(should_choose_best()){
            return this->select_max_score();
        }else{
            uniform_int_distribution<> dis(0,this->levers.size()-1);
            return &(this->levers[dis(gen)]);
        }
    }
    Reward score(L& lever){return lever.mean_reward();}
};


template<class Reward, class Entry, template<typename,typename> class LT>
class EpsilonDecreasing : public EpsilonBandit<Reward,Entry,LT> {
    bool should_choose_best(){
        return this->initial_epsilon/this->get_play() < generate_canonical<double,10>(this->gen);
    }
public:
    EpsilonDecreasing(double initial_epsilon) : EpsilonBandit<Reward,Entry,LT>(initial_epsilon){};
    ~EpsilonDecreasing(){};
};

template<class Reward, class Entry, template<typename,typename> class LT>
class EpsilonGreedy : public EpsilonBandit<Reward,Entry,LT> {
    bool should_choose_best(){
        return this->initial_epsilon < generate_canonical<double,10>(this->gen);
    }
public:
    EpsilonGreedy(double initial_epsilon) : EpsilonBandit<Reward,Entry,LT>(initial_epsilon){};
    ~EpsilonGreedy(){};
};

template<class Reward, class Entry, template<typename,typename> class LT>
class RandomBandit : public EpsilonGreedy<Reward,Entry,LT> {
    bool should_choose_best(){return false;}
public:
    RandomBandit(){};
    ~RandomBandit(){};
};



#include "bandit.cc"

#endif
