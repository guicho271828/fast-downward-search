// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_BANDIT_LIST_H

#include "../evaluation_context.h"
#include "../option_parser.h"
#include "../scalar_evaluator.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <tuple>

using namespace std;

template<class Entry, template<class,class,template<class,class> class> class B>
BanditOpenList<Entry,B>::BanditOpenList(const Options &opts)
    : AbstractTieBreakingOpenList<Entry>(opts){
}

template<class Entry, template<class,class,template<class,class> class> class B>
void BanditOpenList<Entry,B>::do_insertion(
    EvaluationContext &eval_context, const Entry &entry) {

    auto key = AbstractTieBreakingOpenList<Entry>::get_key(eval_context);
    auto plateau = get_plateau(key);
    f_buckets[key] = plateau;

    GlobalState current = eval_context.get_state();
    const SearchSpace &space = eval_context.get_space();
    auto pid = space.get_parent_id(current);
    
    auto &info = depthdb[current];
    int depth;

    if(pid == StateID::no_state){
        assert(current.get_id() == g_initial_state().get_id());
        BucketLever<double,Entry> &lever = (*plateau)[0];
        lever.push(entry);
        info = depthinfo(key,0,--lever.end());
        ++size;
    }else{
        auto parent = g_state_registry->lookup_state(pid);
        auto pinfo = depthdb[parent];
        auto pkey = pinfo.key;
        auto ances_plateau = get_plateau(pkey);
        if ( pkey >= key ){
            // Rewarding previous depth
            // cout << "O";
            ances_plateau->do_reward(1.0);
            depth = pinfo.depth+1;
        }else { assert(pkey < key);
            // Punishing previous depth
            // cout << "X";
            ances_plateau->do_reward(0.0);
            depth = 0;
        }
        
        int oldsize = plateau->levers.size();
        BucketLever<double,Entry> &lever = (*plateau)[depth];
        int newsize = plateau->levers.size();
        if (oldsize < newsize){
            // cout << endl;
            cout << "New depth " << depth << "@" << key << ": ";
            plateau->dump();
        }

        if (! info.initialized){
            lever.push(entry);
            info = depthinfo(key,depth,--lever.end());
            ++size;
        }else{
            // this path is not only taken by reinsert_open, but also
            // by reopen_closed. When reopened, info.key > key.
            // When reinserted, info.key == key. info.key < key never happen.
            assert(info.key >= key);
            if ( info.key > key ){
                // reopened.
                // 
                // Push to the new lever regardress of depth, since f is
                // different. The node in the higher f may be revisited by
                // remove_min, but at that time it should be
                // already closed.
                lever.push(entry);
                info = depthinfo(key,depth,--lever.end());
            }else {
                // reinserted.
                if ( info.depth < depth ){
                    // cout << "updating the depth " << info.depth << " -> " << depth << endl ; 
                    // revisiting the closed node is ok. I do not try to remove the node from lower depth.
                    // however, I do want to stop inserting the node when the new depth is same as or lower than the original node.
                    lever.push(entry);
                    info = depthinfo(key,depth,--lever.end());
                }
            }
            // do not increase the size
        }
    }
}

template<class Entry, template<class,class,template<class,class> class> class B>
Entry BanditOpenList<Entry,B>::remove_min(vector<int> *key) {
    assert(!key); // not supported 
    auto it = f_buckets.begin();
    assert(it != f_buckets.end());
    assert(size > 0);
    auto plateau = it->second;
    assert (! plateau->empty());
    BucketLever<double,Entry> *lever = plateau->select();
    assert (! lever->empty());
    Entry result = lever->pull();
    // cout << endl << "Pulling ";
    // plateau->dump();
    // Note: the expansion of the pulled result may contain closed nodes.
    // When all nodes are closed, no OX indicators will be printed
    --size;
    if (plateau->empty()){
        if (OpenList<Entry>::emit_frontier){
            cout << it->first << " exhausted. " << endl;
            // plateau->dump();
        }
        // this f value is exhausted!
        f_buckets.erase(it);
        if (OpenList<Entry>::emit_frontier){
            cout << "frontier_size=" << frontier_size() << " evals=";
            if (f_buckets.begin()!=f_buckets.end()){
                cout << f_buckets.begin()->first;
                cout << " Next Plateau: " ;
                f_buckets.begin()->second->dump();
            }else{
                cout << "unknown" << endl;
            }
        }
    }
    return result;
}

template<class Entry, template<class,class,template<class,class> class> class B>
bool BanditOpenList<Entry,B>::empty() const {
    // add code here
    return f_buckets.begin() == f_buckets.end();
}

template<class Entry, template<class,class,template<class,class> class> class B>
void BanditOpenList<Entry,B>::clear() {
    // add code here
    f_buckets.clear();
    size = 0;
}

template<class Entry, template<class,class,template<class,class> class> class B>
int BanditOpenList<Entry,B>::frontier_size() {
    auto it = f_buckets.begin();
    if (it == f_buckets.end())
        return 0;
    else{
        return it->second->size();
    }
}

#endif
