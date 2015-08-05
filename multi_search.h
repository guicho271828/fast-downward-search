#ifndef MULTI_SEARCH_H
#define MULTI_SEARCH_H

#include "search_engine.h"
#include "open_lists/open_list.h"

#include <vector>

using namespace std;

class GlobalOperator;
class Heuristic;
class Options;
class ScalarEvaluator;

class MultiSearch : public SearchEngine {

    vector<SearchEngine *> engines;

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit MultiSearch(const Options &opts);
    virtual ~MultiSearch() = default;

    virtual void print_statistics() const override;

    void dump_search_space() const;
};

#endif
