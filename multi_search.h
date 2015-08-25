/* -*- mode : c++ -*- */
#ifndef MULTI_SEARCH_H
#define MULTI_SEARCH_H

#include "search_engine.h"
#include "open_lists/open_list.h"
#include "global_state.h"
#include "global_operator.h"

#include <vector>
#include <deque>
#include <unordered_map>
#include <tuple>

using namespace std;

class GlobalOperator;
class Heuristic;
class Options;
class ScalarEvaluator;

class MultiSearch : public SearchEngine {

    SearchEngine* current_engine;
    vector<SearchEngine *> engines;
    typedef tuple<const GlobalState,
                  const GlobalState,
                  const GlobalOperator *,
                  const bool> PerNodeArgs;
    // typedef tuple<const GlobalState&,
    //               const GlobalState&,
    //               const GlobalOperator *&,
    //               const bool&> PerNodeArgs;
    unordered_map<SearchEngine *, deque<PerNodeArgs > > expanded;
protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit MultiSearch(const Options &opts);
    virtual ~MultiSearch() = default;

    virtual void print_statistics() const override;
    void per_node(const GlobalState &succ, const GlobalState &state, const GlobalOperator *op, const bool is_preferred);
    void dump_search_space() const;
};

#endif
