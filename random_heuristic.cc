#include "random_heuristic.h"

#include "global_state.h"
#include "option_parser.h"
#include "plugin.h"
#include "task_tools.h"
#include <random>
#include <cstddef>
#include <limits>
#include <utility>

using namespace std;

// normalized_computation_distance

RandomHeuristic::RandomHeuristic(const Options &opts)
    : Heuristic(opts) {
  // constructor
}

RandomHeuristic::~RandomHeuristic() {}

void RandomHeuristic::initialize() {
    cout << "Initializing random heuristic..." << endl;
    mt.seed(1);
}

int RandomHeuristic::compute_heuristic(const GlobalState &global_state) {
  return abs(mt());
}

static Heuristic *_parse(OptionParser &parser) {
    parser.document_synopsis("Random heuristic", "Returns a random value");
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
      return new RandomHeuristic(opts);
}

static Plugin<Heuristic> _plugin("random", _parse);
