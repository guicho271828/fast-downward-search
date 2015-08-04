
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

#include <cassert>
#include <cstdlib>
#include <set>
using namespace std;

EagerSearch::EagerSearch(const Options &opts)
    : SearchEngine(opts),
      open_list(opts.get<OpenList<StateID> *>("open")),
      f_evaluator(opts.get<ScalarEvaluator *>("f_eval", nullptr)),
      preferred_operator_heuristics(opts.get_list<Heuristic *>("preferred")) {
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

    // add heuristics that are used in the f_evaluator. They are usually also
    // used in the open list and hence already be included, but we want to be
    // sure.
    if (f_evaluator) {
        f_evaluator->get_involved_heuristics(hset);
    }

    heuristics.assign(hset.begin(), hset.end());
    assert(!heuristics.empty());

    const GlobalState &initial_state = g_initial_state();
    // Note: we consider the initial state as reached by a preferred
    // operator.
    EvaluationContext eval_context(initial_state, 0, true, &statistics);
    hcaches[initial_state] = eval_context.get_cache();

    statistics.inc_evaluated_states();

    if (open_list->is_dead_end(eval_context)) {
        cout << "Initial state is a dead end." << endl;
    } else {
        if (search_progress.check_progress(eval_context))
            print_checkpoint_line(0);
        start_f_value_statistics(eval_context);
        SearchNode node = search_space.get_node(initial_state);
        node.open_initial(eval_context.get_heuristic_value(heuristics[0]));

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
}

SearchStatus EagerSearch::step() {
    pair<SearchNode, bool> n = fetch_next_node();
    if (!n.second) {
        return FAILED;
    }
    SearchNode node = n.first;

    GlobalState s = node.get_state();
    if (check_goal_and_set_plan(s))
        return SOLVED;

    vector<const GlobalOperator *> applicable_ops;
    set<const GlobalOperator *> preferred_ops;

    g_successor_generator->generate_applicable_ops(s, applicable_ops);
    EvaluationContext eval_context(s, node.get_g(), false, &statistics);
    for (Heuristic *heur : preferred_operator_heuristics) {
        if (!eval_context.is_heuristic_infinite(heur)) {
            vector<const GlobalOperator *> preferred =
                eval_context.get_preferred_operators(heur);
            preferred_ops.insert(preferred.begin(), preferred.end());
        }
    }

    for (const GlobalOperator *op : applicable_ops) {
        if ((node.get_real_g() + op->get_cost()) >= bound)
            continue;
        auto succ_state = g_state_registry->get_successor_state(s, *op);
        statistics.inc_generated();
        auto succ_node = search_space.get_node(succ_state);
        per_node(succ_node, node, op, (preferred_ops.find(op) != preferred_ops.end())) ;
    }
    return IN_PROGRESS;
}

void EagerSearch::per_node(SearchNode succ_node,
                           SearchNode node,
                           const GlobalOperator *op,
                           const bool is_preferred){
        if (succ_node.is_dead_end())
    return;

        if (succ_node.is_new()) {
    EvaluationContext
      eval_context(succ_node.get_state(),
                   node.get_g() + get_adjusted_cost(*op),
                   is_preferred,
                   &statistics);
    hcaches[succ_node.get_state()] = eval_context.get_cache();
            statistics.inc_evaluated_states();
            succ_node.clear_h_dirty();

            if (open_list->is_dead_end(eval_context)) {
                succ_node.mark_as_dead_end();
                statistics.inc_dead_ends();
      return;
            }

    succ_node.open(
      eval_context.get_heuristic_value(heuristics[0]), node, op);

    open_list->insert(eval_context, succ_node.get_state().get_id());
    // eval_context.get_cache().dump();
            if (search_progress.check_progress(eval_context)) {
                print_checkpoint_line(succ_node.get_g());
                reward_progress();
            }
        } else if (succ_node.get_g() > node.get_g() + get_adjusted_cost(*op)) {
          // We found a new cheapest path to an open or closed state.          
          if (succ_node.is_closed()) {
            statistics.inc_reopened();
          }
          succ_node.reopen(node, op);

    HeuristicCache hcache = hcaches[succ_node.get_state()];
          EvaluationContext eval_context(hcache, succ_node.get_g(), is_preferred, &statistics);
    open_list->insert(eval_context, succ_node.get_state().get_id());
    // hcache.dump();
    }
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
        update_f_value_statistics(node);
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

void EagerSearch::start_f_value_statistics(EvaluationContext &eval_context) {
    if (f_evaluator) {
        int f_value = eval_context.get_heuristic_value(f_evaluator);
        statistics.report_f_value_progress(f_value);
    }
}

void EagerSearch::update_f_value_statistics(const SearchNode &node) {
    if (f_evaluator) {
        /*
          TODO: This code doesn't fit the idea of supporting
          an arbitrary f evaluator.
        */
        int new_f_value = node.get_g() + node.get_h();
        statistics.report_f_value_progress(new_f_value);
    }
}

static SearchEngine *_parse(OptionParser &parser) {
    //open lists are currently registered with the parser on demand,
    //because for templated classes the usual method of registering
    //does not work:
    Plugin<OpenList<StateID> >::register_open_lists();

    parser.document_synopsis("Eager best-first search", "");

    parser.add_option<OpenList<StateID> *>("open", "open list");
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

    EagerSearch *engine = nullptr;
    if (!parser.dry_run()) {
        engine = new EagerSearch(opts);
    }

    return engine;
}

static Plugin<SearchEngine> _plugin("eager", _parse);
