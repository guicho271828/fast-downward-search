// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_OPEN_TREE_H

#include "../evaluation_context.h"
#include "../option_parser.h"
#include "../scalar_evaluator.h"

#include <iostream>
#include <cassert>
#include <limits>

using namespace std;

template<class Entry>
OpenTree<Entry>::OpenTree(const Options &opts)
    : AbstractTieBreakingOpenList<Entry>(opts) {
}

template<class Entry>
void OpenTree<Entry>::do_insertion(
    EvaluationContext &eval_context, const Entry &entry) {
    vector<GlobalState> path;
    GlobalState current = eval_context.get_state();
    const SearchSpace &space = eval_context.get_space();
    space.trace_path(current, path);
    assert(path[0].get_id()==g_initial_state().get_id());
    auto key = AbstractTieBreakingOpenList<Entry>::get_key(eval_context);
    auto db = get_tree(key);
    // cout << "For key " << key << endl ;

    TreeNode<Entry>* parent = nullptr;
    TreeNode<Entry>* node   = nullptr;
    for (const GlobalState state : path){
        if (! parent){
            // first iteration
            parent = get_node(db,state);
        }else{
            node = get_node(db,state);
            if (node->parent && node->parent != parent){
                // the path have changed
                auto old = node->parent;
                old->children.erase(node);
                cleanup(old,db);
            }
            parent->children.insert(node);
            node->parent = parent;
            parent = node;
        }
    }
    node = (*db)[current];
    if (! node->get_entry()){
        node->set_entry(new Entry(entry)); // freed in remove_min
    }
    // cout << "inserting " << entry << endl;
    ++size;
    if (OpenList<Entry>::emit_frontier)
        counts[key]++;
}

template<class Entry>
TreeNode<Entry>* get_node(DB<Entry>* db, const GlobalState &state){
    auto result = (*db)[state];
    if (! result){
        result = new TreeNode<Entry>(state); // freed in cleanup()
        // cout << "generated node for state " << state.get_id() << " @ " << result << endl;
        (*db)[state] = result;
    }
    return result;
}

template<class Entry>
DB<Entry>* OpenTree<Entry>::get_tree(Key &key){
    auto ptr = trees[key];
    if (ptr){
        return ptr;
    }else{
        // cout << "generating db for key " << key << endl;
        ptr = new DB<Entry>(nullptr); // freed in remove_min
        trees[key] = ptr;
        return ptr;
    }
}

template<class Entry>
Entry OpenTree<Entry>::remove_min(vector<int> *key) {
    assert(!key); // not supported 
    assert(!empty());
    assert(trees.size() > 0);
    auto it = trees.begin();
    assert(it != trees.end());

    auto db = it->second;
    auto root = (*db)[g_initial_state()];
    assert(root);

    --size;
    if (OpenList<Entry>::emit_frontier)
        counts[it->first]--;
    // root->dump();
    auto result = search_and_cleanup(root,db);
    // cout << "-->" << endl;
    // root->dump();
    // cout << "removed " << *result << endl;
    if (root->children.empty()){
        // this f value is exhausted!
        trees.erase(it);
        delete root;
        delete db;
        if (OpenList<Entry>::emit_frontier){
            cout << "frontier_size=" << frontier_size() << " evals=";
            if (trees.begin()!=trees.end()){
                cout << trees.begin()->first << endl;
            }else{
                cout << "unknown" << endl;
            }
        }
    }
    return result;
}

template<class Entry>
const Entry OpenTree<Entry>::search_and_cleanup(TreeNode<Entry>* tree, DB<Entry>* db) {
    auto node = search(tree);
    auto result = *(node->get_entry()); // move
    // assert(node->children.empty());
    // not true;; root-help-help-open-open is possible
    node->delete_entry();
    // becomes    root-help-help-help-open.
    // still somewhat fifo
    cleanup(node, db);
    return result;
}

template<class Entry>
TreeNode<Entry>* OpenTree<Entry>::search(TreeNode<Entry>* tree) {
    if (tree->get_entry()){
        // TODO: if there are children like root-help-help-open(1)-open(2),
        // is it necessary to open (1) and (2) at random?
        // If I change this condition to tree->children.empty(),
        // then it would open (2) first.
        return tree;
    }
    else{
        assert(!tree->children.empty());
        std::uniform_int_distribution<> dis(0, tree->children.size()-1);
        auto it = tree->children.begin();
        int i = dis(gen);
        for (int j = 0 ; j < i ; j++) it++;
        return search(*it);
    }
}

template<class Entry>
void cleanup(TreeNode<Entry>* tree, DB<Entry>* db) {
    if (tree->parent // not a root node
        && tree->children.empty()
        && !(tree->get_entry())){
        tree->parent->children.erase(tree);
        cleanup(tree->parent,db);
        (*db)[tree->state] = nullptr;
        delete tree;
    }
}

template<class Entry>
bool OpenTree<Entry>::empty() const {
    // add code here
    return trees.begin() == trees.end();
}

template<class Entry>
void OpenTree<Entry>::clear() {
    // add code here
    trees.clear();
    size = 0;
}

template<class Entry>
int OpenTree<Entry>::frontier_size() {
    auto it = counts.begin();
    if (it == counts.end())
        return 0;
    else
        return it->second;
}

template<class Entry>
OpenList<Entry> *OpenTree<Entry>::_parse(OptionParser &parser) {
    parser.document_synopsis("Open Tree", "Automatically maintain diversity");
    parser.add_list_option<ScalarEvaluator *>("evals", "scalar evaluators");
    parser.add_option<bool>(
        "pref_only",
        "insert only nodes generated by preferred operators", "false");
    parser.add_option<bool>(
        "unsafe_pruning",
        "allow unsafe pruning when the main evaluator regards a state a dead end",
        "false");
    parser.add_option<int>("queue", "queue order, 0:fifo,1:lifo,2:random", "0");
    parser.add_option<bool>("frontier", "Print the size of the frontier when new one is visited", "false");
    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new OpenTree<Entry>(opts);
}

#endif
