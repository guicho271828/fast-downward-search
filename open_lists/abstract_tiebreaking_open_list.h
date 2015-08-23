#ifndef OPEN_LISTS_ABSTRACT_TIEBREAKING_OPEN_LIST_H
#define OPEN_LISTS_ABSTRACT_TIEBREAKING_OPEN_LIST_H

#include "open_list.h"
#include "../rng.h"

#include <deque>
#include <map>
#include <utility>
#include <vector>

class OptionParser;
class Options;
class ScalarEvaluator;

enum QueueType {FIFO,LIFO,RANDOM_O};


template<class Entry>
class AbstractTieBreakingOpenList : public OpenList<Entry> {


protected:
    std::vector<ScalarEvaluator *> evaluators;
    /*
      If allow_unsafe_pruning is true, we ignore (don't insert) states
      which the first evaluator considers a dead end, even if it is
      not a safe heuristic.
    */
    bool allow_unsafe_pruning;
    int dimension() const {return evaluators.size();};

public:
    explicit AbstractTieBreakingOpenList(const Options &opts);
    virtual ~AbstractTieBreakingOpenList() override = default;
    virtual void get_involved_heuristics(
        std::set<Heuristic *> &hset) override;
    virtual bool is_dead_end(
        EvaluationContext &eval_context) const override;
    virtual bool is_reliable_dead_end(
        EvaluationContext &eval_context) const override;
    vector<int> get_key(EvaluationContext &eval_context);
    const int queue;
};

#include "abstract_tiebreaking_open_list.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
