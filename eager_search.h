#ifndef EAGER_SEARCH_H
#define EAGER_SEARCH_H

#include "search_engine.h"

#include "open_lists/open_list.h"

#include <vector>

class GlobalOperator;
class Heuristic;
class Options;
class ScalarEvaluator;

class EagerSearch : public SearchEngine {

    bool complete_search;
    bool reinsert_open;
    std::vector<Heuristic *> heuristics;
    std::vector<Heuristic *> preferred_operator_heuristics;

protected:
    OpenList<StateID> *open_list;
    void reward_progress();
    void per_node(const GlobalState &succ, const GlobalState &state, const GlobalOperator *op, const bool is_preferred);
    void per_node_new(const GlobalState &succ, const GlobalState &state, const GlobalOperator *op, const bool is_preferred);
    void per_node_reopen(const GlobalState &succ, const GlobalState &state, const GlobalOperator *op, const bool is_preferred);
    void per_node_update_parent(const GlobalState &succ, const GlobalState &state, const GlobalOperator *op, const bool is_preferred);
    void print_checkpoint_line(int g) const;
    virtual std::pair<SearchNode, bool> fetch_next_node();

public:
    explicit EagerSearch(const Options &opts);
    virtual ~EagerSearch() = default;
    virtual void initialize() override;
    virtual SearchStatus step() override;
    virtual void save_plan_if_necessary() const override;
    virtual void print_statistics() const override;

    void dump_search_space() const;
};

#endif
