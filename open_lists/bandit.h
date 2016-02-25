/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_BANDIT_H
#define OPEN_LISTS_BANDIT_H
#include <limits>
// #include <deque>
#include "../rng.h"
#include <unordered_map>
#include <map>
#include <cassert>
#include <iostream>
#include <deque>

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
    virtual void push(const Entry &e) = 0;
    virtual bool available() = 0;
};


template<class Reward, class Entry, template<typename,typename> class LT>
class Bandit {
    typedef LT<Reward,Entry> L;
    int play = 0;
    L* last_selected = nullptr;
public:
    map<int,L,greater<int>> levers;
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
        for (auto &lever : this->levers){
            if (lever.second.available()){
                Reward s = score(lever.second);
                if (s == numeric_limits<double>::infinity())
                    return &(lever.second);
                if (max < s){
                    max = s;
                    best = &(lever.second);
                }
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
protected:
public:
    Reward k;
    UCB(){};
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
class RandomBandit : public Bandit<Reward,Entry,LT> {
    typedef LT<Reward,Entry> L;
protected:
    RandomNumberGenerator rng;
public:
    explicit RandomBandit(){};
    ~RandomBandit(){};
    L* do_select(){
        deque<L*> availables;
        for (auto &lever : this->levers){
            if (lever.second.available()){
                availables.push_back(&(lever.second));
            }
        }
        assert(!availables.empty());
        int i = rng(availables.size());
        return availables[i];
    };
    Reward score(L& lever){
        Reward x = numeric_limits<Reward>::quiet_NaN();
        return x;
    };
};

template<class Reward, class Entry, template<typename,typename> class LT>
class LoopingBandit : public Bandit<Reward,Entry,LT> {
    typedef LT<Reward,Entry> L;
protected:
    int i = 0;
public:
    explicit LoopingBandit(){};
    ~LoopingBandit(){};
    L* do_select(){
        deque<L*> availables;
        for (auto &lever : this->levers){
            if (lever.second.available()){
                availables.push_back(&(lever.second));
            }
        }
        assert(!availables.empty());
        i++;
        if (i>=availables.size()){
            i=0;
        }
        return availables[i];
    };
    Reward score(L& lever){
        Reward x = numeric_limits<Reward>::quiet_NaN();
        return x;
    };
};


template<class Reward, class Entry, template<typename,typename> class LT>
class EpsilonBandit : public RandomBandit<Reward,Entry,LT> {
    typedef LT<Reward,Entry> L;
protected:
    Reward initial_epsilon;
    virtual bool should_choose_best() = 0;
public:
    explicit EpsilonBandit(Reward initial_epsilon) : initial_epsilon(initial_epsilon){};
    ~EpsilonBandit(){};
    L* do_select(){
        if(should_choose_best()){
            return this->select_max_score();
        }else{
            return RandomBandit<Reward,Entry,LT>::do_select();
        }
    }
    Reward score(L& lever){return lever.mean_reward();}
};


template<class Reward, class Entry, template<typename,typename> class LT>
class EpsilonDecreasing : public EpsilonBandit<Reward,Entry,LT> {
    bool should_choose_best(){
        return this->initial_epsilon/this->get_play() < generate_canonical<Reward,10>(this->gen);
    }
public:
    explicit EpsilonDecreasing(Reward initial_epsilon) : EpsilonBandit<Reward,Entry,LT>(initial_epsilon){};
    ~EpsilonDecreasing(){};
};

template<class Reward, class Entry, template<typename,typename> class LT>
class EpsilonGreedy : public EpsilonBandit<Reward,Entry,LT> {
    bool should_choose_best(){
        return this->initial_epsilon < generate_canonical<Reward,10>(this->gen);
    }
public:
    explicit EpsilonGreedy(Reward initial_epsilon) : EpsilonBandit<Reward,Entry,LT>(initial_epsilon){};
    ~EpsilonGreedy(){};
};

template<class Reward, class Entry, template<typename,typename> class LT>
class FirstBandit : public Bandit<Reward,Entry,LT> {
    typedef LT<Reward,Entry> L;
public:
    explicit FirstBandit(){};
    ~FirstBandit(){};
    L* do_select(){
        // since levers are sorted according to greater<int>
        L* last = nullptr;
        for (auto &lever : this->levers){
            if (lever.second.available()){
                // smallest index/depth
                last = &(lever.second);
            }
        }
        return last;
    };
    Reward score(L& lever){
        Reward x = numeric_limits<Reward>::quiet_NaN();
        return x;
    };
};

template<class Reward, class Entry, template<typename,typename> class LT>
class LastBandit : public Bandit<Reward,Entry,LT> {
    typedef LT<Reward,Entry> L;
public:
    explicit LastBandit(){};
    ~LastBandit(){};
    L* do_select(){
        for (auto &lever : this->levers){
            if (lever.second.available()){
                // largest index/depth
                return &(lever.second);
            }
        }
        assert(false);
        return nullptr;
    };
    Reward score(L& lever){
        Reward x = numeric_limits<Reward>::quiet_NaN();
        return x;
    };
};



#include "bandit.cc"

#endif
