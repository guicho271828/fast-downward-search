#ifndef SEARCH_STATISTICS_H
#define SEARCH_STATISTICS_H

/*
  This class keeps track of search statistics.

  It keeps counters for expanded, generated and evaluated states (and
  some other statistics) and provides uniform output for all search
  methods.
*/

class SearchStatistics {
    // General statistics
    int expanded_states;  // no states for which successors were generated
    int evaluated_states; // no states for which h fn was computed
    int generated_states; // no states created in total (plus those removed since already in close list)
    int reopened_states;  // no of *closed* states which we reopened
    int dead_end_states;

    int generated_ops;    // no of operators that were returned as applicable
    int evaluations;      // no of heuristic evaluations performed

    // Statistics related to f values
    int lastjump_f_value; //f value obtained in the last jump
    int lastjump_expanded_states; // same guy but at point where the last jump in the open list
    int lastjump_reopened_states; // occurred (jump == f-value of the first node in the queue increases)
    int lastjump_evaluated_states;
    int lastjump_generated_states;

    void print_f_line() const;
public:
    SearchStatistics();
    ~SearchStatistics() = default;

    // Methods that update statistics.
    void inc_expanded(int inc = 1) {expanded_states += inc; }
    void inc_evaluated_states(int inc = 1) {evaluated_states += inc; }
    void inc_generated(int inc = 1) {generated_states += inc; }
    void inc_reopened(int inc = 1) {reopened_states += inc; }
    void inc_dead_ends(int inc = 1) {dead_end_states += inc; }

    void inc_generated_ops(int inc = 1) {generated_ops += inc; }
    void inc_evaluations(int inc = 1) {evaluations += inc; }

    // Methods that access statistics.
    int get_expanded() const {return expanded_states; }
    int get_evaluated_states() const {return evaluated_states; }
    int get_generated() const {return generated_states; }
    int get_reopened() const {return reopened_states; }
    int get_dead_ends() const {return dead_end_states; }

    int get_generated_ops() const {return generated_ops; }
    int get_evaluations() const {return evaluations; }

    SearchStatistics &operator+(const SearchStatistics &stat){
        SearchStatistics* r = new SearchStatistics();
        r->inc_expanded(get_expanded()+stat.get_expanded());
        r->inc_evaluated_states(get_evaluated_states()+stat.get_evaluated_states());
        r->inc_generated(get_generated()+stat.get_generated());
        r->inc_reopened(get_reopened()+stat.get_reopened());
        r->inc_dead_ends(get_dead_ends()+stat.get_dead_ends());
    
        r->inc_generated_ops(get_generated_ops()+stat.get_generated_ops());
        r->inc_evaluations(get_evaluations()+stat.get_evaluations());
        return *r;
    }
    SearchStatistics &operator+=(const SearchStatistics &stat) {
        inc_expanded(stat.get_expanded());
        inc_evaluated_states(stat.get_evaluated_states());
        inc_generated(stat.get_generated());
        inc_reopened(stat.get_reopened());
        inc_dead_ends(stat.get_dead_ends());
    
        inc_generated_ops(stat.get_generated_ops());
        inc_evaluations(stat.get_evaluations());
        return *this;
    }

    /*
      Call the following method with the f value of every expanded
      state. It will notice "jumps" (i.e., when the expanded f value
      is the highest f value encountered so far), print some
      statistics on jumps, and keep track of expansions etc. up to the
      last jump.

      Statistics until the final jump are often useful to report in
      A*-style searches because they are not affected by tie-breaking
      as the overall statistics. (With a non-random, admissible and
      consistent heuristic, the number of expanded, evaluated and
      generated states until the final jump is fully determined by the
      state space and heuristic, independently of things like the
      order in which successors are generated or the tie-breaking
      performed by the open list.)
    */
    void report_f_value_progress(int f);

    // output
    void print_basic_statistics() const;
    void print_detailed_statistics() const;
};

#endif
