#include "option_parser.h"
#include "plugin.h"
#include "multi_eager_search.h"

MultiEagerSearch::MultiEagerSearch(const Options &opts)
    : EagerSearch(opts),
      open_list (SepaltOpenList<StateID>(opts.get_list<OpenList<StateID> *>("sublists"),0)){
}

void MultiEagerSearch::insert_initial(GlobalState initial){
    for ( SearchSpace space : search_spaces ){
        EagerSearch::insert_initial(initial);
    }
}

std::pair<SearchNode, bool> MultiEagerSearch::fetch_next_node(){

    auto node = EagerSearch::fetch_next_node();
    search_space = search_spaces[open_list.current()];
    return node;
}

void MultiEagerSearch::print_statistics() const {

    statistics.print_detailed_statistics();
    for ( SearchSpace space : search_spaces ){
        space.print_statistics();
    }
}

static SearchEngine *_parse(OptionParser &parser) {
    Plugin<OpenList<StateID> >::register_open_lists();
    parser.document_synopsis("Multi-queue Eager best-first search with caching heuristic values", "");
    
    parser.add_list_option<OpenList<StateID> *>("sublists", "open lists", "[]");
    parser.add_option<ScalarEvaluator *>(
        "f_eval",
        "set evaluator for jump statistics. "
        "(Optional; if no evaluator is used, jump statistics will not be displayed.)",
        OptionParser::NONE);
    parser.add_list_option<Heuristic *>(
        "preferred",
        "use preferred operators of these heuristics", "[]");
    SearchEngine::add_options_to_parser(parser);
    Options opts = parser.parse();
    opts.verify_list_non_empty<OpenList<StateID> *>("sublists");

    MultiEagerSearch *engine = nullptr;
    if (!parser.dry_run()) {
        engine = new MultiEagerSearch(opts);
    }

    return engine;
}

static Plugin<SearchEngine> _plugin("multi_eager", _parse);
