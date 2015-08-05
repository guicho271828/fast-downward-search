#include "heuristic_cache.h"
#include "globals.h"

using namespace std;


HeuristicCache::HeuristicCache(const GlobalState &state)
    : state(state) {
}

EvaluationResult &HeuristicCache::operator[](ScalarEvaluator *heur) {
    return eval_results[heur];
}

const GlobalState &HeuristicCache::get_state() const {
    return state;
}

void HeuristicCache::dump() const {
  cout << "Heuristic Cache: State :" << endl;
  state.dump_pddl();
  cout << "Results of heuristics in " << eval_results.size() << " ScalarEvaluators :" << endl;

  for_each_heuristic_value(
    [] (const Heuristic * heur, const EvaluationResult &result) {
      cout << "Value for "
           << heur->get_description() << ": ";
      if (result.is_infinite())
        cout << "infinity";
      else
        cout << result.get_h_value();
      cout << endl;
    });
}
