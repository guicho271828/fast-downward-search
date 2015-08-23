
#ifdef OPEN_LISTS_TREENODE_H
#include "../rng.h"

template<class Entry>
void TreeNode<Entry>::delete_entry(){
    if (entry){
        delete entry;
        entry = nullptr;
    }
}

template<class Entry>
void TreeNode<Entry>::dump(int level){
    cout << ";; ";
    for (int i = 0; i < level; i++)
        cout << "  ";
    if (entry){
        cout << *entry << " @ " << this << endl ;
    }else{
        cout << "null" << " @ " << this << endl ;
    }
    for (auto child : children){
        child->dump(level+1);
    }
}

template<class Entry>
const Entry TreeNode<Entry>::search_and_cleanup(DB<Entry>* db, int queue) {
    TreeNode<Entry>* node = nullptr;
    switch (queue){
    case FIFO:
        node = search_first_entry();
        break;
    case LIFO:
        node = search_last_entry();
        break;
    case RANDOM_O:
        node = search_random_depth();
        break;
    default:
        assert(false);
    }
    auto result = *(node->get_entry()); // move
    // assert(node->children.empty());
    // not true;; root-help-help-open-open is possible
    node->delete_entry();
    // becomes    root-help-help-help-open.
    // still somewhat fifo
    node->cleanup(db);
    return result;
}

template<class Entry>
TreeNode<Entry>* TreeNode<Entry>::search_first_entry() {
    if (get_entry()){
        return this;
    }
    assert(!children.empty());
    auto it = children.begin();
    int i = g_rng(children.size());
    for (int j = 0 ; j < i ; j++) it++;
    return (*it)->search_first_entry();
}

template<class Entry>
TreeNode<Entry>* TreeNode<Entry>::search_last_entry() {
    if (get_entry() && children.empty()){
        return this;
    }
    assert(!children.empty());
    auto it = children.begin();
    int i = g_rng(children.size());
    for (int j = 0 ; j < i ; j++) it++;
    return (*it)->search_last_entry();
}

template<class Entry>
TreeNode<Entry>* TreeNode<Entry>::search_random_depth() {
    return search_random_depth_rec(new vector<TreeNode<Entry> *>());
}

template<class Entry>
TreeNode<Entry>* TreeNode<Entry>::search_random_depth_rec(vector<TreeNode<Entry> *>* acc) {
    if (get_entry()){
        if(acc->size()==acc->capacity())
            acc->resize(2*acc->size());
        acc->push_back(this);
        if (children.empty()){
            return *(g_rng.choose<TreeNode<Entry> *>(*acc));
        }
    }
    assert(!children.empty());
    auto it = children.begin();
    int i = g_rng(children.size());
    for (int j = 0 ; j < i ; j++) it++;
    return (*it)->search_random_depth_rec(acc);
}

template<class Entry>
void TreeNode<Entry>::cleanup(DB<Entry>* db) {
    if (parent // not a root node
        && children.empty()
        && !(get_entry())){
        
        parent->children.erase(this);
        parent->cleanup(db);
        (*db)[state] = nullptr;
        delete this;
    }
}


#endif
