/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_BANDIT_LIST_H
#define OPEN_LISTS_BANDIT_LIST_H

#include "abstract_tiebreaking_open_list.h"
#include "../per_state_information.h"
#include "bandit.h"
#include <limits>
#include <unordered_map>
#include <tuple>
#include <list>

using namespace std;


template<class Reward, class Entry>
class BucketLever : public Lever<Reward,Entry> {
public:
    // BucketLever() : Lever<Reward,Entry>() {};
    BucketLever(){}
    ~BucketLever(){}
    list<Entry> bucket;
    Entry pull() {
        Entry e = bucket.back();
        bucket.pop_back();
        return e;
    }
    void push(Entry e) {
        // emulates lifo
        bucket.push_back(e);
    }
    bool empty(){return bucket.empty();}
    int size(){return bucket.size();}
    void erase(typename list<Entry>::iterator it){
        bucket.erase(it);
    }
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
    UCBPlateau(Reward k):UCB<Reward,Entry,BucketLever>(k){};
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
    void dump() {
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



template<class Entry>
class UCBOpenList : public AbstractTieBreakingOpenList<Entry> {
    int size = 0;
    const double k;
    typedef vector<int> Key;
    typedef UCBPlateau<double,Entry> P;
    map<Key, P*> f_buckets;
    unordered_map<Key, P*> f_buckets_unordered;

    struct depthinfo {
        depthinfo() : initialized(false){};
        depthinfo(Key key, int depth,
                  typename list<Entry>::iterator it,
                  bool initialized)
            : key(key), depth(depth), it(it), initialized(initialized){};
        Key key;
        int depth;
        typename list<Entry>::iterator it;
        bool initialized;
    };
    PerStateInformation<depthinfo> depthdb;
    P* get_plateau(Key key){
        P* ptr = f_buckets_unordered[key];
        if (!ptr){
            ptr = new UCBPlateau<double,Entry>(k);
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
