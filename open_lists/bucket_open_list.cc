// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_BUCKET_OPEN_LIST_H

#include "../option_parser.h"
#include "../scalar_evaluator.h"

#include <cassert>
#include <limits>

using namespace std;

template<class Entry>
BucketOpenList<Entry>::BucketOpenList(const Options &opts)
    : OpenList<Entry>(opts.get<bool>("pref_only"),
                      opts.get<bool>("fifo")),
      lowest_bucket(numeric_limits<int>::max()),
      size(0),
      evaluator(opts.get<ScalarEvaluator *>("eval")) {
}

template<class Entry>
void BucketOpenList<Entry>::do_insertion(
    EvaluationContext &eval_context, const Entry &entry) {
    int key = eval_context.get_heuristic_value(evaluator);
    assert(key >= 0);
    int num_buckets = buckets.size();
    if (key >= num_buckets)
        buckets.resize(key + 1);
    if (key < lowest_bucket)
        lowest_bucket = key;
    if (this->fifo)
      buckets[key].push_back(entry);
    else
      buckets[key].push_front(entry);
    ++size;
}

template<class Entry>
Entry BucketOpenList<Entry>::remove_min(vector<int> *key) {
    assert(size > 0);
    while (buckets[lowest_bucket].empty())
        ++lowest_bucket;
    --size;
    if (key) {
        assert(key->empty());
        key->push_back(lowest_bucket);
    }
    Entry result = buckets[lowest_bucket].front();
    buckets[lowest_bucket].pop_front();
    return result;
}

template<class Entry>
bool BucketOpenList<Entry>::empty() const {
    return size == 0;
}

template<class Entry>
void BucketOpenList<Entry>::clear() {
    buckets.clear();
    lowest_bucket = numeric_limits<int>::max();
    size = 0;
}

template<class Entry>
void BucketOpenList<Entry>::get_involved_heuristics(
    std::set<Heuristic *> &hset) {
    evaluator->get_involved_heuristics(hset);
}

template<class Entry>
bool BucketOpenList<Entry>::is_dead_end(
    EvaluationContext &eval_context) const {
    return eval_context.is_heuristic_infinite(evaluator);
}

template<class Entry>
bool BucketOpenList<Entry>::is_reliable_dead_end(
    EvaluationContext &eval_context) const {
    return is_dead_end(eval_context) && evaluator->dead_ends_are_reliable();
}

template<class Entry>
OpenList<Entry> *BucketOpenList<Entry>::_parse(OptionParser &parser) {
    parser.document_synopsis(
        "Bucket-based open list",
        "Bucket-based open list implementation that uses a single evaluator. "
        "Ties are broken in FIFO order.");
    parser.add_option<ScalarEvaluator *>("eval", "scalar evaluator");
    parser.add_option<bool>(
        "pref_only",
        "insert only nodes generated by preferred operators",
        "false");
    parser.add_option<bool>("fifo", "insert in fifo order", "true");
    
    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new BucketOpenList<Entry>(opts);
}

template<class Entry>
int BucketOpenList<Entry>::frontier_size() {
    return buckets[lowest_bucket].size();
}


#endif
