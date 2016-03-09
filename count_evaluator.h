#ifndef COUNT_EVALUATOR_H
#define COUNT_EVALUATOR_H

/* originates from weighted evaluator */

#include "scalar_evaluator.h"

class Options;

class CountEvaluator : public ScalarEvaluator {
    int limit;
    int current;

public:
    explicit CountEvaluator(const Options &opts);
    CountEvaluator(int limit,int current);
    virtual ~CountEvaluator() override;

    virtual bool dead_ends_are_reliable() const override;
    virtual EvaluationResult compute_result(EvaluationContext &eval_context) override;
    virtual void get_involved_heuristics(std::set<Heuristic *> &hset) override;
};

#endif
