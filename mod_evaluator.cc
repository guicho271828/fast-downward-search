#include "mod_evaluator.h"

#include "evaluation_context.h"
#include "evaluation_result.h"
#include "option_parser.h"
#include "plugin.h"

#include <cstdlib>
#include <sstream>

ModEvaluator::ModEvaluator(const Options &opts)
    : evaluator(opts.get<ScalarEvaluator *>("eval")),
      w(opts.get<int>("divisor")) {
}

ModEvaluator::ModEvaluator(ScalarEvaluator *eval, int divisor)
    : evaluator(eval), w(divisor) {
}


ModEvaluator::~ModEvaluator() {
}

bool ModEvaluator::dead_ends_are_reliable() const {
    return evaluator->dead_ends_are_reliable();
}

EvaluationResult ModEvaluator::compute_result(
    EvaluationContext &eval_context) {
    // Note that this produces no preferred operators.
    EvaluationResult result;
    int h_val = eval_context.get_heuristic_value_or_infinity(evaluator);
    if (h_val != EvaluationResult::INFINITE) {
        // TODO: Check for overflow?
        h_val /= w;
    }
    result.set_h_value(h_val);
    return result;
}

void ModEvaluator::get_involved_heuristics(std::set<Heuristic *> &hset) {
    evaluator->get_involved_heuristics(hset);
}

static ScalarEvaluator *_parse(OptionParser &parser) {
    parser.document_synopsis(
        "Mod evaluator",
        "Divide the value of the scalar evaluator with the given divisor. Remainders are ignored.");
    parser.add_option<ScalarEvaluator *>("eval", "scalar evaluator");
    parser.add_option<int>("divisor", "divisor");
    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new ModEvaluator(opts);
}

static Plugin<ScalarEvaluator> _plugin("divisor", _parse);
