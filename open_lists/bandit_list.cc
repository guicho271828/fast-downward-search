// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_BANDIT_LIST_H

#include "../evaluation_context.h"
#include "../option_parser.h"
#include "../scalar_evaluator.h"

#include <iostream>
#include <cassert>
#include <limits>

using namespace std;

template<class Entry>
UCBOpenList<Entry>::UCBOpenList(const Options &opts)
    : AbstractTieBreakingOpenList<Entry>(opts),
      k(opts.get<double>("k")){
    cout << "Initializing UCB openlist with exploration ratio = " << k << endl;
}

template<class Entry>
void UCBOpenList<Entry>::do_insertion(
    EvaluationContext &eval_context, const Entry &entry) {

    auto key = AbstractTieBreakingOpenList<Entry>::get_key(eval_context);
    auto plateau = get_plateau(key);
    f_buckets[key] = plateau;

    GlobalState current = eval_context.get_state();
    const SearchSpace &space = eval_context.get_space();
    auto pid = space.get_parent_id(current);
    
    int depth;
    if (pid == StateID::no_state){
        assert(current.get_id() == g_initial_state().get_id());
        depth = 0;
    }else{
        auto parent = g_state_registry->lookup_state(pid);
        auto pair = depthdb[parent];
        auto pkey = pair.first;
        if (pkey == key){
            depth = pair.second + 1;
            assert(plateau==get_plateau(pkey));
            // cout << "O";
            // cout << "rewarding depth " << pair.second << endl;
            plateau->do_reward(1.0);
        }else{
            depth = 0;
            // cout << "X" ;
            // cout << "Punishing depth " << pair.second << endl;
            auto prev_plateau = get_plateau(pkey);
            prev_plateau->do_reward(0.0);
        }
    }
    int oldsize = plateau->levers.size();
    BucketLever<double,Entry> &lever = (*plateau)[depth];
    int newsize = plateau->levers.size();
    lever.push(entry);
    if (oldsize < newsize){
        // cout << endl;
        cout << "New depth " << depth << "@" << key << ": ";
        plateau->dump();
    }
    depthdb[current] = make_pair(key,depth);
    ++size;
}

template<class Entry>
Entry UCBOpenList<Entry>::remove_min(vector<int> *key) {
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
                cout << "unknown";
            }
        }
    }
    return result;
}

template<class Entry>
bool UCBOpenList<Entry>::empty() const {
    // add code here
    return f_buckets.begin() == f_buckets.end();
}

template<class Entry>
void UCBOpenList<Entry>::clear() {
    // add code here
    f_buckets.clear();
    size = 0;
}

template<class Entry>
int UCBOpenList<Entry>::frontier_size() {
    auto it = f_buckets.begin();
    if (it == f_buckets.end())
        return 0;
    else{
        return it->second->size();
    }
}

template<class Entry>
OpenList<Entry> *UCBOpenList<Entry>::_parse(OptionParser &parser) {
    parser.document_synopsis("UCB open List", "Adaptive Queue which learns the depth to open next within same f");
    parser.add_list_option<ScalarEvaluator *>("evals", "scalar evaluators");
    parser.add_option<bool>(
        "pref_only",
        "insert only nodes generated by preferred operators", "false");
    parser.add_option<bool>(
        "unsafe_pruning",
        "allow unsafe pruning when the main evaluator regards a state a dead end",
        "false");
    parser.add_option<bool>("frontier", "Print the size of the frontier when new one is visited", "false");
    parser.add_option<double>("k", "Exploration ratio k of UCB index \\mu + \\sqrt{ k ln(n)/n_i }. k<0.5 is unsafe.", "0.5");
    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new UCBOpenList<Entry>(opts);
}

#endif
