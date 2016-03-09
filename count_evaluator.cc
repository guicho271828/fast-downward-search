#include "count_evaluator.h"

#include "evaluation_context.h"
#include "evaluation_result.h"
#include "option_parser.h"
#include "plugin.h"

#include <cstdlib>
#include <sstream>

CountEvaluator::CountEvaluator(const Options &opts)
    : limit(opts.get<int>("limit")),current(opts.get<int>("initial")) {
}

CountEvaluator::CountEvaluator(int limit,int current)
    : limit(limit),current(current) {
}

CountEvaluator::~CountEvaluator() {
}

EvaluationResult CountEvaluator::compute_result(
    EvaluationContext &eval_context) {

    current++;
    if (current >= limit){
        current = 0;
    }
    EvaluationResult result;
    result.set_h_value(current);
    return result;
}

void CountEvaluator::get_involved_heuristics(std::set<Heuristic *> &hset) {
}
bool CountEvaluator::dead_ends_are_reliable () const {
    return true;
}

static ScalarEvaluator *_parse(OptionParser &parser) {
    parser.document_synopsis(
        "Count evaluator",
        "Returns an integer from 0 to LIMIT-1, where LIMIT is a given integer. First time it returns 0, then 1, 2, 3 and so on.");
    parser.add_option<int>("limit", "upper limit", "1");
    parser.add_option<int>("initial", "initial value", "0");
    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new CountEvaluator(opts);
}

static Plugin<ScalarEvaluator> _plugin("count", _parse);
