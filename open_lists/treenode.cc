
#ifdef OPEN_LISTS_TREENODE_H

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
int TreeNode<Entry>::max_depth() {
    int max = -1;
    for ( auto &subtree : children ){
        int depth = subtree->max_depth();
        if (max < depth)
            max = depth;
    }
    return max + 1;
}

template<class Entry>
vector<TreeNode<Entry>*> TreeNode<Entry>::branches_with_depth(int depth) {
    vector<TreeNode<Entry>*> branches(children.size());
    for ( auto &subtree : children ){
        if (depth < subtree->max_depth()){
            branches.push_back(subtree);
        }
    }
    return branches;
}


template<class Entry>
const Entry TreeNode<Entry>::search_and_cleanup(DB<Entry>* db, int queue) {
    auto node = search(queue);
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
TreeNode<Entry>* TreeNode<Entry>::search(int queue) {
    if (get_entry()){
        switch (queue){
        case FIFO:
            return this;
        case LIFO:
            if (!children.empty()){
                return this;
            }
            break;
        case RANDOM_O:{
            int depth = g_rng(max_depth());
            auto branches = branches_with_depth(depth);
            auto it = g_rng.choose<TreeNode<Entry>* >(branches);
            return (**it).search(queue);
        }
        }
    }
    
    assert(!children.empty());
    auto it = children.begin();
    int i = g_rng(children.size());
    for (int j = 0 ; j < i ; j++) it++;
    return (*it)->search(queue);
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
