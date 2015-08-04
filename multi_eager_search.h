#ifndef MULTI_EAGER_SEARCH_H
#define MULTI_EAGER_SEARCH_H

#include "eager_search.h"
#include "open_lists/sepalt_open_list.h"

class MultiEagerSearch : public EagerSearch {

    std::vector<SearchSpace> search_spaces;

protected:
    SepaltOpenList<StateID> *open_list;
    /* virtual void initialize() override; */
    /* virtual SearchStatus step() override; */
    virtual std::pair<SearchNode, bool> fetch_next_node() override;
    virtual void insert_initial(GlobalState initial) override;
    
public:
    explicit MultiEagerSearch(const Options &opts);
    virtual ~MultiEagerSearch() = default;

    /* virtual void per_node(SearchNode succ_node, */
    /*                       SearchNode node, */
    /*                       const GlobalOperator *op, */
    /*                       bool is_preferred) override ; */
};

#endif
