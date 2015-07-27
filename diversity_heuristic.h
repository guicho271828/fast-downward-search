#ifndef DIVERSITY_HEURISTIC_H
#define DIVERSITY_HEURISTIC_H

#include "heuristic.h"

class DiversityHeuristic : public Heuristic {
    int min_operator_cost;
protected:
    virtual void initialize();
    virtual int compute_heuristic(const GlobalState &global_state);
public:
    DiversityHeuristic(const Options &options);
    ~DiversityHeuristic();
};

#endif
