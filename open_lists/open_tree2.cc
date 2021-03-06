// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_OPEN_TREE2_H

#include "open_tree2.h"
#include <climits>
#include "../option_parser.h"

using namespace std;

template<class Entry>
OpenDTree<Entry>::OpenDTree(const Options &opts)
    : OpenTree<Entry>(opts),
      count_helpers(opts.get<bool>("helpers")),
      count_open(opts.get<bool>("open")) {
    if ((!opts.get<bool>("helpers")) && (!opts.get<bool>("open"))){
        throw ArgError("either helpers or open should be true");
    }
}

template<class Entry>
const Entry OpenDTree<Entry>::search_and_cleanup(TreeNode<Entry>* tree, DB<Entry>* db) {
    count_cache.clear();
    return OpenTree<Entry>::search_and_cleanup(tree, db);
}

template<class Entry>
TreeNode<Entry>* OpenDTree<Entry>::search(TreeNode<Entry>* tree) {
    if (tree->get_entry()){
        assert(tree->children.empty());
        return tree;
    } else{
        assert(!tree->children.empty());
        TreeNode<Entry>* min_b = nullptr;
        int min = INT_MAX;
        for (auto branch : tree->children ){
            int count = count_branch(branch);
            if (count < min){
                min = count;
                min_b = branch;
            }
        }
        assert(min_b);
        return search(min_b);
    }
}

template<class Entry>
int OpenDTree<Entry>::count_branch(TreeNode<Entry>* tree) {
    auto found = count_cache.find(tree);
    if(found != count_cache.end()) {
        return found->second;
    }
    if (tree->get_entry()){
        int count = count_open ? 1 : 0;
        count_cache[tree] = count;
        return count;
    } else{
        assert(!tree->children.empty());
        int count = count_helpers ? 1 : 0;
        for ( auto branch : tree->children) {
            count += count_branch(branch);
        }
        count_cache[tree] = count;
        return count;
    }
}

template<class Entry>
OpenList<Entry> *OpenDTree<Entry>::_parse(OptionParser &parser) {
    parser.document_synopsis("Open DTree", "Deterministically maintain diversity");
    parser.add_list_option<ScalarEvaluator *>("evals", "scalar evaluators");
    parser.add_option<bool>(
        "pref_only",
        "insert only nodes generated by preferred operators", "false");
    parser.add_option<bool>(
        "unsafe_pruning",
        "allow unsafe pruning when the main evaluator regards a state a dead end",
        "false");
    parser.add_option<bool>("frontier", "Print the size of the frontier when new one is visited", "false");
    parser.add_option<bool>("helpers", "Count the helper nodes", "false");
    parser.add_option<bool>("open", "Count the open nodes", "true");
    parser.add_option<int>("queue", "queue order, 0:fifo,1:lifo,2:random", "0");
    Options opts = parser.parse();

    if (parser.dry_run())
        return 0;
    else
        return new OpenDTree<Entry>(opts);
}

#endif
