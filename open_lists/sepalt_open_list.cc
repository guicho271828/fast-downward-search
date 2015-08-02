#ifdef OPEN_LISTS_SEPALT_OPEN_LIST_H

#include "../evaluation_context.h"
#include "../option_parser.h"

#include <cassert>
#include <cstdlib>
using namespace std;


template<class Entry>
SepaltOpenList<Entry>::SepaltOpenList(const Options &opts)
  : AlternationOpenList<Entry>(opts) {
}

template<class Entry>
void SepaltOpenList<Entry>::do_insertion(
    EvaluationContext &eval_context, const Entry &entry) {
  AlternationOpenList<Entry>::open_lists[best]->insert(eval_context, entry);
}

template<class Entry>
Entry SepaltOpenList<Entry>::remove_min(vector<int> *key) {
    if (key) {
        cerr << "not implemented -- see msg639 in the tracker" << endl;
        exit_with(EXIT_UNSUPPORTED);
    }
    best = AlternationOpenList<Entry>::best_list_index();
    OpenList<Entry> *best_list = AlternationOpenList<Entry>::open_lists[best];
    assert(!best_list->empty());
    ++AlternationOpenList<Entry>::priorities[best];
    return best_list->remove_min(nullptr);
}

template<class Entry>
OpenList<Entry> *SepaltOpenList<Entry>::_parse(OptionParser &parser) {
    parser.document_synopsis("Separated Alternation open list",
                             "Same as alternation open list, but the successor nodes are only inserted to itself");
    parser.add_list_option<OpenList<Entry> *>(
        "sublists",
        "open lists between which this one alternates");
    parser.add_option<int>(
        "boost",
        "boost value for contained open lists that are restricted "
        "to preferred successors",
        "0");

    Options opts = parser.parse();
    opts.verify_list_non_empty<OpenList<Entry> *>("sublists");
    if (parser.dry_run())
        return nullptr;
    else
        return new SepaltOpenList<Entry>(opts);
}

#endif
