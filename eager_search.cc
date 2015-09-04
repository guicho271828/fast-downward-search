
// note: simplified from the original source code by
// 
// 1. removing the multi-path dependence
// 2. always assuming reopening the node
// 3. removing "eager_greedy", "astar"

#include "eager_search.h"
  
#include "evaluation_context.h"
#include "g_evaluator.h"
#include "globals.h"
#include "heuristic.h"
#include "option_parser.h"
#include "plugin.h"
#include "successor_generator.h"
#include "sum_evaluator.h"

#include <typeinfo>
#include <cxxabi.h>
#include <cassert>
#include <cstdlib>
#include <set>
using namespace std;

EagerSearch::EagerSearch(const Options &opts)
  : SearchEngine(opts),
    open_list(opts.get<OpenList<StateID> *>("open")),
    preferred_operator_heuristics(opts.get_list<Heuristic *>("preferred")),
    reinsert_open(opts.get<bool>("reinsert_open")){
}

void EagerSearch::initialize() {
  cout << "Conducting best first search"
       << " with reopening closed nodes, (real) bound = " << bound
       << endl;
  assert(open_list);

  set<Heuristic *> hset;
  open_list->get_involved_heuristics(hset);

  // add heuristics that are used for preferred operators (in case they are
  // not also used in the open list)
  hset.insert(preferred_operator_heuristics.begin(),
              preferred_operator_heuristics.end());

  heuristics.assign(hset.begin(), hset.end());
  assert(!heuristics.empty());

  const GlobalState &initial_state = g_initial_state();
  // Note: we consider the initial state as reached by a preferred
  // operator.
  EvaluationContext eval_context =
      get_context(initial_state, 0, true, &statistics, &search_space);
  statistics.inc_evaluated_states();

  if (open_list->is_dead_end(eval_context)) {
    cout << "Initial state is a dead end." << endl;
  } else {
    if (search_progress.check_progress(eval_context))
        print_checkpoint_line(0);
    auto node = search_space.get_node(initial_state);
    node.open_initial();
    open_list->insert(eval_context, initial_state.get_id());
  }
  print_initial_h_values(eval_context);
}


void EagerSearch::print_checkpoint_line(int g) const {
  cout << "[g=" << g << ", ";
  statistics.print_basic_statistics();
  cout << "]" << endl;
}

void EagerSearch::print_statistics() const {
  statistics.print_detailed_statistics();
  search_space.print_statistics();
  set<Heuristic *> hset;
  open_list->get_involved_heuristics(hset);
  for ( auto h : hset ){
      int     status;
      char   *realname;
      realname = abi::__cxa_demangle(typeid(*h).name(), 0, 0, &status);
      // std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
      cout << "TrueEvaluation " << realname << " " << h << " " << h->true_evaluation_count << endl;
      free(realname);
  }
}

SearchStatus EagerSearch::step() {
  auto n = fetch_next_node();

  if (!n.second)
    return FAILED;
  auto node = n.first;
  assert(!node.is_dead_end());
  assert(node.is_closed());
  
  auto s = node.get_state();
  if (check_goal_and_set_plan(s))
    return SOLVED;
  vector<const GlobalOperator *> applicable_ops;
  set<const GlobalOperator *> preferred_ops;
  g_successor_generator->generate_applicable_ops(s, applicable_ops);
  EvaluationContext eval_context = get_context(s, node.get_g(), false, &statistics, &search_space);
  for (Heuristic *heur : preferred_operator_heuristics) {
    if (!eval_context.is_heuristic_infinite(heur)) {
      auto preferred = eval_context.get_preferred_operators(heur);
      preferred_ops.insert(preferred.begin(), preferred.end());
    }
  }
  for (const GlobalOperator *op : applicable_ops) {
    if ((node.get_real_g() + op->get_cost()) >= bound)
      continue;
    auto succ = g_state_registry->get_successor_state(s, *op);
    statistics.inc_generated();
    main_engine->per_node(succ, s, op, (preferred_ops.find(op) != preferred_ops.end())) ;
  }
  return IN_PROGRESS;
}

bool EagerSearch::per_node(const GlobalState &succ,
                           const GlobalState &state,
                           const GlobalOperator *op,
                           const bool is_preferred){
    auto succ_node = search_space.get_node(succ);
    auto node = search_space.get_node(state);
    if (succ_node.is_dead_end())
        return false;

    if (succ_node.is_new()) {
        return per_node_new(succ,state,op,is_preferred);
    } else if (succ_node.get_g() > node.get_g() + get_adjusted_cost(*op)) {
        return per_node_reopen(succ,state,op,is_preferred);
    } else if (reinsert_open &&
               (succ_node.get_g() == node.get_g() + get_adjusted_cost(*op))){
        // since h is path-independent and f = g+h, f is also the same !
        return per_node_update_parent(succ,state,op,is_preferred);
    } else {
        // closed and do not find a shorter path
        return false;
    }
}

bool EagerSearch::per_node_new(const GlobalState &succ,
                               const GlobalState &state,
                               const GlobalOperator *op,
                               const bool is_preferred){
    auto succ_node = search_space.get_node(succ);
    auto node = search_space.get_node(state);
    EvaluationContext eval_context =
        get_context(succ,
                     node.get_g() + get_adjusted_cost(*op),
                     is_preferred, &statistics, &search_space);

    if (open_list->is_dead_end(eval_context)) {
        succ_node.mark_as_dead_end();
        statistics.inc_dead_ends();
    }else{
        succ_node.open(node, op);
        open_list->insert(eval_context, succ.get_id());
        if (search_progress.check_progress(eval_context)) {
            print_checkpoint_line(succ_node.get_g());
            reward_progress();
        }
    }
    statistics.inc_evaluated_states();
    return true;
}

bool EagerSearch::per_node_reopen(const GlobalState &succ,
                                  const GlobalState &state,
                                  const GlobalOperator *op,
                                  const bool is_preferred){
    auto succ_node = search_space.get_node(succ);
    auto node = search_space.get_node(state);
    // We found a new cheapest path to an open or closed state.
    if (succ_node.is_closed()){
        statistics.inc_reopened();
    }
    succ_node.reopen(node, op);
    EvaluationContext eval_context =
        get_context(succ, succ_node.get_g(), is_preferred, &statistics, &search_space);
    open_list->insert(eval_context, succ.get_id());
    
    return false;
}

bool EagerSearch::per_node_update_parent(const GlobalState &succ,
                                         const GlobalState &state,
                                         const GlobalOperator *op,
                                         const bool is_preferred){
    auto succ_node = search_space.get_node(succ);
    auto node = search_space.get_node(state);
    // We found a new parent within the same plateau.
    if (!succ_node.is_closed()){
        succ_node.update_parent(node, op);
        EvaluationContext eval_context =
            get_context(succ, succ_node.get_g(), is_preferred, &statistics, &search_space);
        // Assuming the queue is lifo, reinsert the node to the open list
        // so that the latest information is used
        open_list->insert(eval_context, succ.get_id());
    }    
    return false;
}

pair<SearchNode, bool> EagerSearch::fetch_next_node() {
  while (true) {
    if (open_list->empty()) {
      cout << "Completely explored state space -- no solution!" << endl;
      // HACK! HACK! we do this because SearchNode has no default/copy constructor
      SearchNode dummy_node = search_space.get_node(g_initial_state());
      return make_pair(dummy_node, false);
    }
    StateID id = open_list->remove_min(nullptr);
    // TODO is there a way we can avoid creating the state here and then
    //      recreate it outside of this function with node.get_state()?
    //      One way would be to store GlobalState objects inside SearchNodes
    //      instead of StateIDs
    GlobalState s = g_state_registry->lookup_state(id);
    SearchNode node = search_space.get_node(s);

    if (node.is_closed())
      continue;

    node.close();
    assert(!node.is_dead_end());
    statistics.inc_expanded();
    return make_pair(node, true);
  }
}

void EagerSearch::reward_progress() {
  // Boost the "preferred operator" open lists somewhat whenever
  // one of the heuristics finds a state with a new best h value.
  open_list->boost_preferred();
}

void EagerSearch::dump_search_space() const {
  search_space.dump();
}

static SearchEngine *_parse(OptionParser &parser) {
  //open lists are currently registered with the parser on demand,
  //because for templated classes the usual method of registering
  //does not work:
  Plugin<OpenList<StateID> >::register_open_lists();

  parser.document_synopsis("Eager best-first search", "");

  parser.add_option<OpenList<StateID> *>("open", "open list");
  parser.add_list_option<Heuristic *>("preferred", "use preferred operators of these heuristics", "[]");
  parser.add_option<bool>("reopen_closed", "This option is ignored.", "true");
  parser.add_option<bool>("reinsert_open", "Always reinsert the open node to the open list", "false");
  SearchEngine::add_options_to_parser(parser);
  Options opts = parser.parse();

  EagerSearch *engine = nullptr;
  if (!parser.dry_run()) {
    engine = new EagerSearch(opts);
  }

  return engine;
}

static Plugin<SearchEngine> _plugin("eager", _parse);
