#ifndef OPEN_LISTS_SEPALT_OPEN_LIST_H
#define OPEN_LISTS_SEPALT_OPEN_LIST_H

#include "alternation_open_list.h"
/* #include "../multi_eager_search.h" */
class Options;
class OptionParser;

#include <vector>

template<class Entry>
class SepaltOpenList : public AlternationOpenList<Entry> {
    int best = 0;
    /* friend MultiEagerSearch; */
    
protected:
    virtual void do_insertion(EvaluationContext &eval_context,
                              const Entry &entry) override;
public:
    explicit SepaltOpenList(const Options &opts);
    SepaltOpenList(const std::vector<OpenList<Entry> *> &sublists,
                   int boost_amount);
    virtual ~SepaltOpenList() override = default;
    virtual Entry remove_min(std::vector<int> *key = 0) override;
    int current(){ return best; };
    static OpenList<Entry> *_parse(OptionParser &parser);
};

#include "sepalt_open_list.cc"

#endif
