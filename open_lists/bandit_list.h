/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_BANDIT_LIST_H
#define OPEN_LISTS_BANDIT_LIST_H

#include "abstract_tiebreaking_open_list.h"
#include "../per_state_information.h"
#include "bandit.h"
#include <limits>
#include <unordered_map>
#include <deque>

using namespace std;


template<class Reward, class Entry>
class BucketLever : public Lever<Reward,Entry> {
    deque<Entry> bucket;
public:
    int queue;
    BucketLever(){}
    ~BucketLever(){}
    Entry pull() {
        Entry result;
        switch (queue){
        case FIFO:
            result = bucket.front();
            bucket.pop_front();
            break;
        case LIFO:
            result = bucket.back();
            bucket.pop_back();
            break;
        case RANDOM_O:{
            int i = g_rng(bucket.size());
            auto it = bucket.begin()+i;
            result = *it;
            bucket.erase(it);
            break;
        }
        }
        return result;
    }
    void push(const Entry &e) {
            // emulates lifo
            bucket.push_back(e);
        }
    bool empty(){return bucket.empty();}
    int size(){return bucket.size();}
    void erase(typename deque<Entry>::iterator it){
        bucket.erase(it);
    }
    typename deque<Entry>::iterator end(){
        return bucket.end();
    }
    bool available(){
        return !bucket.empty();
    };
};

template<class Reward, class Entry, template<class,class,template<class,class> class> class B>
class Plateau : public B<Reward,Entry,BucketLever> {
    typedef BucketLever<Reward,Entry> BL;
public:
    int queue;
    Plateau():B<Reward,Entry,BucketLever>(){};
    ~Plateau(){};
    bool empty(){
        for (auto &lever : this->levers){
            if (!lever.second.empty())
                return false;
        }
        return true;
    };
    int size(){
        int s = 0;
        for (auto &lever : this->levers){
            s += lever.second.size();
        }
        return s;
    };
    void dump(){
        Reward max = -1 * numeric_limits<double>::infinity();
        int i = 0;
        int best_index = 0;
        cout << "[" ;
        auto it = this->levers.rbegin();
        bool first = true;
        while (it!=this->levers.rend()){
            Reward s = this->score(it->second);
            if (!first)
                cout << ", ";
            cout << s << "/" << it->second.size();
            first=false;
            if (!it->second.empty()){
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

template<class Entry, template<class,class,template<class,class> class> class B>
class BanditOpenList : public AbstractTieBreakingOpenList<Entry> {
public:
    typedef vector<int> Key;
    typedef Plateau<double,Entry,B> P;
private:
    int size = 0;
    map<Key, P*> f_buckets;
    unordered_map<Key, P*> f_buckets_unordered;
    struct depthinfo {
        depthinfo() : key(Key()),
                      initialized(false),
                      depth(-1){
        };
        depthinfo(const Key &key, const int &depth)
            : key(key), depth(depth), initialized(true){};
        Key key;
        int depth = -1;
        bool initialized = false;
    };
    PerStateInformation<depthinfo> depthdb;
    P* get_plateau(const Key &key){
        P* ptr = f_buckets_unordered[key];
        if (!ptr){
            ptr = make_plateau();
            f_buckets_unordered[key] = ptr;
        }
        return ptr;
    }
    
protected:
    virtual void do_insertion(EvaluationContext &eval_context,
                              const Entry &entry) override;
    virtual P* make_plateau() = 0;
public:
    explicit BanditOpenList(const Options &opts);
    ~BanditOpenList() override = default;
    virtual Entry remove_min(vector<int> *key = 0) override;
    /* low priority */
    virtual bool empty() const override;
    virtual void clear() override;
    virtual int frontier_size() override;
};



#include "bandit_list.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
