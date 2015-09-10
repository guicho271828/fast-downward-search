// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_TIEBREAKING_OPEN_LIST_H

#include "../evaluation_context.h"
#include "../option_parser.h"
#include "../scalar_evaluator.h"
#include "../rng.h"

#include <iostream>
#include <cassert>
#include <limits>

using namespace std;


template<class Entry>
OpenList<Entry> *TieBreakingOpenList<Entry>::_parse(OptionParser &parser) {
    parser.document_synopsis("Tie-breaking open list", "");
    parser.add_list_option<ScalarEvaluator *>("evals", "scalar evaluators");
    parser.add_option<bool>(
        "pref_only",
        "insert only nodes generated by preferred operators", "false");
    parser.add_option<bool>(
        "unsafe_pruning",
        "allow unsafe pruning when the main evaluator regards a state a dead end",
        "false");
    parser.add_option<int>("queue", "queue order, 0:fifo,1:lifo,2:random", "0");
    parser.add_option<int>("seed", "seed for randomness.", "1");
    parser.add_option<bool>("minstd", "use minstd. ignored", "false");
    parser.add_option<bool>("frontier", "Print the size of the frontier when new one is visited", "false");
    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new TieBreakingOpenList<Entry>(opts);
}

template<class Entry>
TieBreakingOpenList<Entry>::TieBreakingOpenList(const Options &opts)
    : AbstractTieBreakingOpenList<Entry>(opts),
      size(0), evaluators(opts.get_list<ScalarEvaluator *>("evals")),
      allow_unsafe_pruning(opts.get<bool>("unsafe_pruning")) {
    rng.seed(opts.get<int>("seed"));
}

template<class Entry>
TieBreakingOpenList<Entry>::TieBreakingOpenList(
    const std::vector<ScalarEvaluator *> &evals,
    bool preferred_only, bool unsafe_pruning)
    : OpenList<Entry>(preferred_only), size(0), evaluators(evals),
      allow_unsafe_pruning(unsafe_pruning) {
}

template<class Entry>
void TieBreakingOpenList<Entry>::do_insertion(
    EvaluationContext &eval_context, const Entry &entry) {
    auto key = AbstractTieBreakingOpenList<Entry>::get_key(eval_context);
    buckets[key].push_back(entry);
    ++size;
}

template<class Entry>
Entry TieBreakingOpenList<Entry>::remove_min(vector<int> *key) {
    assert(size > 0);
    auto it = buckets.begin();
    assert(it != buckets.end());
    assert(!it->second.empty());
    --size;
    if (key) {
        assert(key->empty());
        *key = it->first;
    }
    auto &bucket = it->second;

    Entry result = bucket.front();
    switch (this->queue){
    case FIFO:
        bucket.pop_front();
        break;
    case LIFO:
        result = bucket.back();
        bucket.pop_back();
        break;
    case RANDOM_O:{
        int i = rng(bucket.size());
        auto it = bucket.begin()+i;
        result = *it;
        *it = bucket.back();
        bucket.pop_back();
        break;
    }
    default:
        assert(false);
    }
    if (bucket.empty()){
        buckets.erase(it);
        if (OpenList<Entry>::emit_frontier)
            cout << "frontier_size=" << frontier_size() << " evals=" << buckets.begin()->first << endl;
    }
    return result;
}

template<class Entry>
bool TieBreakingOpenList<Entry>::empty() const {
    return size == 0;
}

template<class Entry>
void TieBreakingOpenList<Entry>::clear() {
    buckets.clear();
    size = 0;
}

template<class Entry>
int TieBreakingOpenList<Entry>::dimension() const {
    return evaluators.size();
}

template<class Entry>
void TieBreakingOpenList<Entry>::get_involved_heuristics(
    std::set<Heuristic *> &hset) {
    for (ScalarEvaluator *evaluator : evaluators)
        evaluator->get_involved_heuristics(hset);
}

template<class Entry>
bool TieBreakingOpenList<Entry>::is_dead_end(
    EvaluationContext &eval_context) const {
    // TODO: Properly document this behaviour.
    // If one safe heuristic detects a dead end, return true.
    if (is_reliable_dead_end(eval_context))
        return true;
    // If the first heuristic detects a dead-end and we allow "unsafe
    // pruning", return true.
    if (allow_unsafe_pruning &&
        eval_context.is_heuristic_infinite(evaluators[0]))
        return true;
    // Otherwise, return true if all heuristics agree this is a dead-end.
    for (ScalarEvaluator *evaluator : evaluators)
        if (!eval_context.is_heuristic_infinite(evaluator))
            return false;
    return true;
}

template<class Entry>
bool TieBreakingOpenList<Entry>::is_reliable_dead_end(
    EvaluationContext &eval_context) const {
    for (ScalarEvaluator *evaluator : evaluators)
        if (eval_context.is_heuristic_infinite(evaluator) &&
            evaluator->dead_ends_are_reliable())
            return true;
    return false;
}

template<class Entry>
int TieBreakingOpenList<Entry>::frontier_size() {
    auto it = buckets.begin();
    if (it == buckets.end())
        return 0;
    else
        return it->second.size();
}




#endif
