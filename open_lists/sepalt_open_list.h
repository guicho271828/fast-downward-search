#ifndef OPEN_LISTS_SEPALT_OPEN_LIST_H
#define OPEN_LISTS_SEPALT_OPEN_LIST_H

#include "alternation_open_list.h"
class Options;
class OptionParser;

#include <vector>

template<class Entry>
class SepaltOpenList : public AlternationOpenList<Entry> {
    int best;

protected:
    virtual void do_insertion(EvaluationContext &eval_context,
                              const Entry &entry) override;
public:
    explicit SepaltOpenList(const Options &opts);
    virtual ~SepaltOpenList() override = default;
    virtual Entry remove_min(std::vector<int> *key = 0) override;
    static OpenList<Entry> *_parse(OptionParser &parser);
};

#include "sepalt_open_list.cc"

#endif
