#ifndef RANDOM_HEURISTIC_H
#define RANDOM_HEURISTIC_H

#include "heuristic.h"
#include <random>

class RandomHeuristic : public Heuristic {
  std::mt19937 mt;
protected:
    virtual void initialize();
    virtual int compute_heuristic(const GlobalState &global_state);
public:
    RandomHeuristic(const Options &options);
    ~RandomHeuristic();
};

#endif
