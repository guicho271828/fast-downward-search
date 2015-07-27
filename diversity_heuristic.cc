#include "diversity_heuristic.h"

#include "global_state.h"
#include "option_parser.h"
#include "plugin.h"
#include "task_tools.h"

#include <cstddef>
#include <limits>
#include <utility>
#include <zlib>
using namespace std;

DiversityHeuristic::DiversityHeuristic(const Options &opts)
    : Heuristic(opts) {
    min_operator_cost = numeric_limits<int>::max();
    for (OperatorProxy op : task_proxy.get_operators())
        min_operator_cost = min(min_operator_cost, op.get_cost());
}

DiversityHeuristic::~DiversityHeuristic() {
}

void DiversityHeuristic::initialize() {
    cout << "Initializing diversity heuristic..." << endl;
}

int DiversityHeuristic::compute_heuristic(const GlobalState &global_state) {
    State state = convert_global_state(global_state);
    if (is_goal_state(task_proxy, state))
        return 0;
    else
        return min_operator_cost;
}

static Heuristic *_parse(OptionParser &parser) {
    parser.document_synopsis("Diversity heuristic", "Returns the novelty of the state");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "supported");
    parser.document_language_support("axioms", "supported");
    parser.document_property("admissible", "no");
    parser.document_property("consistent", "no");
    parser.document_property("safe", "no");
    parser.document_property("preferred operators", "no");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run())
      return 0;
    else
      return new DiversityHeuristic(opts);
}

static Plugin<Heuristic> _plugin("diversity", _parse);
