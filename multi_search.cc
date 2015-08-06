#include "multi_search.h"

#include "evaluation_context.h"
#include "g_evaluator.h"
#include "globals.h"
#include "heuristic.h"
#include "option_parser.h"
#include "plugin.h"
#include "successor_generator.h"
#include "sum_evaluator.h"

#include <cassert>
#include <cstdlib>
#include <set>
using namespace std;

MultiSearch::MultiSearch(const Options &opts)
    : SearchEngine(opts),
      engines(opts.get_list<SearchEngine *>("engines")) {
}

void MultiSearch::initialize() {
    for (auto engine : engines){
        engine->initialize(hcaches);
    }
}

SearchStatus MultiSearch::step() {
    uint failed_count = 0;

    for (auto engine : engines){
        switch (engine->step()){
        case SOLVED:
            set_plan(engine->get_plan());
            return SOLVED ;
        case FAILED: failed_count++ ;
        }
    }
    if (failed_count == engines.size())
        return FAILED;
    return IN_PROGRESS;
}

void MultiSearch::print_statistics() const {
    int i = 0;
    SearchStatistics stat;
    for (auto engine : engines){
        cout << "  Search engine " << i << ":" << endl;
        engine->print_statistics();
        i++;
        stat += engine->get_statistics();
    }
    cout << "  Aggregated:" << endl;
    stat.print_detailed_statistics();
}

static SearchEngine *_parse(OptionParser &parser) {
    parser.document_synopsis("Multi search", "Run several search engines independently, but the heuristic computation results are shared.");
    parser.add_list_option<SearchEngine *>("engines", "specify multiple search engines");
    SearchEngine::add_options_to_parser(parser);
    Options opts = parser.parse();
    opts.verify_list_non_empty<SearchEngine *>("engines");

    MultiSearch *engine = nullptr;
    if (!parser.dry_run()) {
        engine = new MultiSearch(opts);
    }

    return engine;
}
static Plugin<SearchEngine> _plugin("multi", _parse);
