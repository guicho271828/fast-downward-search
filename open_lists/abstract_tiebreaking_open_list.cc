// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_ABSTRACT_TIEBREAKING_OPEN_LIST_H

#include "../evaluation_context.h"
#include "../option_parser.h"
#include "../scalar_evaluator.h"

#include <iostream>
#include <cassert>
#include <limits>

using namespace std;

template<class Entry>
AbstractTieBreakingOpenList<Entry>::AbstractTieBreakingOpenList(const Options &opts)
    : OpenList<Entry>(opts.get<bool>("pref_only"), opts.get<bool>("frontier")),
      evaluators(opts.get_list<ScalarEvaluator *>("evals")),
      queue(opts.get<int>("queue")),
      allow_unsafe_pruning(opts.get<bool>("unsafe_pruning")) {
}

template<class Entry>
void AbstractTieBreakingOpenList<Entry>::get_involved_heuristics(
    std::set<Heuristic *> &hset) {
    for (ScalarEvaluator *evaluator : evaluators)
        evaluator->get_involved_heuristics(hset);
}

template<class Entry>
vector<int> AbstractTieBreakingOpenList<Entry>::get_key(
    EvaluationContext &eval_context) {
    vector<int> key;
    key.reserve(evaluators.size());
    for (ScalarEvaluator *evaluator : evaluators)
        key.push_back(eval_context.get_heuristic_value_or_infinity(evaluator));
    return key;
}


template<class Entry>
bool AbstractTieBreakingOpenList<Entry>::is_dead_end(
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
bool AbstractTieBreakingOpenList<Entry>::is_reliable_dead_end(
    EvaluationContext &eval_context) const {
    for (ScalarEvaluator *evaluator : evaluators)
        if (eval_context.is_heuristic_infinite(evaluator) &&
            evaluator->dead_ends_are_reliable())
            return true;
    return false;
}

#endif
