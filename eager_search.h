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

    std::vector<Heuristic *> heuristics;
    std::vector<Heuristic *> preferred_operator_heuristics;


protected:
    PerStateInformation<HeuristicCache> hcaches;
    OpenList<StateID> *open_list;
    virtual void initialize() override;
    virtual SearchStatus step() override;
    virtual std::pair<SearchNode, bool> fetch_next_node();
    void print_checkpoint_line(int g) const;
    void reward_progress();

public:
    explicit EagerSearch(const Options &opts);
    virtual ~EagerSearch() = default;

    virtual void per_node(SearchNode succ_node,
                          SearchNode node,
                          const GlobalOperator *op,
                          bool is_preferred);

    virtual void print_statistics() const override;

    void dump_search_space() const;
};

#endif
