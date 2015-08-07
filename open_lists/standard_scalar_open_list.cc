// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_STANDARD_SCALAR_OPEN_LIST_H

#include "../evaluation_context.h"
#include "../scalar_evaluator.h"
#include "../option_parser.h"

#include <cassert>

using namespace std;


template<class Entry>
StandardScalarOpenList<Entry>::StandardScalarOpenList(const Options &opts)
  : OpenList<Entry>(opts.get<bool>("pref_only")),
    size(0),
    fifo(opts.get<bool>("fifo")),
    evaluator(opts.get<ScalarEvaluator *>("eval")) {
}

template<class Entry>
StandardScalarOpenList<Entry>::StandardScalarOpenList(
    ScalarEvaluator *evaluator, bool preferred_only)
    : OpenList<Entry>(preferred_only),
      size(0),
      evaluator(evaluator) {
}

template<class Entry>
void StandardScalarOpenList<Entry>::do_insertion(
    EvaluationContext &eval_context, const Entry &entry) {
    int key = eval_context.get_heuristic_value(evaluator);
    if (this->fifo)
      buckets[key].push_back(entry);
    else
      buckets[key].push_front(entry);
    ++size;
}

template<class Entry>
Entry StandardScalarOpenList<Entry>::remove_min(vector<int> *key) {
    assert(size > 0);
    auto it = buckets.begin();
    assert(it != buckets.end());
    if (key) {
        assert(key->empty());
        key->push_back(it->first);
    }

    Bucket &bucket = it->second;
    assert(!bucket.empty());
    Entry result = bucket.front();
    bucket.pop_front();
    if (bucket.empty()){
        buckets.erase(it);
        cout << "frontier_size=" << frontier_size() << " evals=[" << buckets.begin()->first << "]" << endl;
    }
    --size;
    return result;
}

template<class Entry>
bool StandardScalarOpenList<Entry>::empty() const {
    return size == 0;
}

template<class Entry>
void StandardScalarOpenList<Entry>::clear() {
    buckets.clear();
    size = 0;
}

template<class Entry>
void StandardScalarOpenList<Entry>::get_involved_heuristics(
    std::set<Heuristic *> &hset) {
    evaluator->get_involved_heuristics(hset);
}

template<class Entry>
bool StandardScalarOpenList<Entry>::is_dead_end(
    EvaluationContext &eval_context) const {
    return eval_context.is_heuristic_infinite(evaluator);
}

template<class Entry>
bool StandardScalarOpenList<Entry>::is_reliable_dead_end(
    EvaluationContext &eval_context) const {
    return is_dead_end(eval_context) && evaluator->dead_ends_are_reliable();
}

template<class Entry>
OpenList<Entry> *StandardScalarOpenList<Entry>::_parse(OptionParser &parser) {
    parser.document_synopsis(
        "Standard open list",
        "Standard open list that uses a single evaluator");
    parser.add_option<ScalarEvaluator *>("eval", "scalar evaluator");
    parser.add_option<bool>(
        "pref_only",
        "insert only nodes generated by preferred operators", "false");
    parser.add_option<bool>("fifo", "insert in fifo order", "true");

    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;
    else
        return new StandardScalarOpenList<Entry>(opts);
}
template<class Entry>
int StandardScalarOpenList<Entry>::frontier_size() {
    auto it = buckets.begin();
    if (it == buckets.end())
        return 0;
    else
        return it->second.size();
}

#endif
