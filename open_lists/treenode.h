/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_TREENODE_H
#define OPEN_LISTS_TREENODE_H

using namespace std;

template<class Entry>
using DB = PerStateInformation<TreeNode<Entry>* > ;


template<class Entry>
class TreeNode {
    // No implementation to forbid copies and assignment
    TreeNode(const TreeNode<Entry> &);
    TreeNode &operator=(const TreeNode<Entry> &);
    Entry* entry;
public:
    explicit TreeNode(const GlobalState &state) : entry(nullptr), state(state){};
    ~TreeNode(){
        /* assert(!parent); */
        assert(!entry);
        assert(children.empty());
    };
    TreeNode* parent = nullptr;
    /* void set_parent(TreeNode* parent2){ assert(!parent); parent = parent2; } */
    Entry* get_entry(){ return entry; };
    void set_entry(Entry* entry2){assert(!entry); entry = entry2;};
    void delete_entry();
    set<TreeNode*> children;
    const GlobalState state;
    void dump(int level = 0);
    int max_depth();
    int search(int queue);
    const Entry search_and_cleanup(DB<Entry>* db, int queue);
    void cleanup(DB<Entry>* db, int queue);
};

#include "treenode.cc"

#endif
