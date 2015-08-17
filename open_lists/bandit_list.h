/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_BANDIT_LIST_H
#define OPEN_LISTS_BANDIT_LIST_H

#include "abstract_tiebreaking_open_list.h"
#include "../per_state_information.h"
#include "bandit.h"
#include <limits>
#include <unordered_map>

using namespace std;


template<class Reward, class Entry>
class BucketLever : public Lever<Reward,Entry> {
public:
    // BucketLever() : Lever<Reward,Entry>() {};
    BucketLever(){};
    ~BucketLever(){};
    deque<Entry> bucket;
    Entry pull() {
        Entry e = bucket.front();
        bucket.pop_front();
        return e;
    };
    void push(Entry e) {
        // emulates lifo
        bucket.push_front(e);
    };
    bool empty(){return bucket.empty();};
    int size(){return bucket.size();};
};



template<class Reward, class Entry>
class UCBPlateau : public UCB<Reward,Entry,BucketLever> { 
    typedef BucketLever<Reward,Entry> BL;
    BL* do_select(){
        assert(!this->levers.empty());
        Reward max = -1 * numeric_limits<Reward>::infinity();
        BL* best = nullptr;
        int i = 0;
        int best_index = 0;
        for (auto &lever : this->levers){
            if (lever.second.empty()){
                // cout << "Slot " << i << " is empty!: " << endl;
            }else{
                Reward s = this->score(lever.second);
                if (max < s){
                    max = s;
                    best = &(lever.second);
                    best_index = i;
                }
            }
            i++;
        }
        assert(best);
        // this->dump();
        return best;
    };
public:
    UCBPlateau(){};
    ~UCBPlateau(){};
    bool empty(){
        for (auto lever : this->levers){
            if (!lever.second.empty())
                return false;
        }
        return true;
    };
    int size(){
        int s = 0;
        for (auto lever : this->levers){
            s += lever.second.size();
        }
        return s;
    };
    BL& operator[](int i){return this->levers[i];};
};



template<class Entry>
class UCBOpenList : public AbstractTieBreakingOpenList<Entry> {
    int size = 0;
    typedef vector<int> Key;
    typedef UCBPlateau<double,Entry> P;
    map<Key, P*> f_buckets;
    unordered_map<Key, P*> f_buckets_unordered;
    PerStateInformation<pair<Key,int>> depthdb;

    P* get_plateau(Key key){
        P* ptr = f_buckets_unordered[key];
        if (!ptr){
            ptr = new UCBPlateau<double,Entry>();
            f_buckets_unordered[key] = ptr;
        }
        return ptr;
    }
    
protected:
    virtual void do_insertion(EvaluationContext &eval_context,
                              const Entry &entry) override;

public:
    explicit UCBOpenList(const Options &opts);
    virtual ~UCBOpenList() override = default;

    virtual Entry remove_min(vector<int> *key = 0) override;
    /* low priority */
    virtual bool empty() const override;
    virtual void clear() override;
    virtual int frontier_size() override;
    static OpenList<Entry> *_parse(OptionParser &parser);
};

#include "bandit_list.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
