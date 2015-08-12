#ifndef OPEN_LISTS_OPEN_TREE2_H
#define OPEN_LISTS_OPEN_TREE2_H

#include "open_tree.h"

using namespace std;

template<class Entry>
class OpenDTree : public OpenTree<Entry> {
    map<TreeNode<Entry>*,int> count_cache;
protected:

public:
    explicit OpenDTree(const Options &opts);
    virtual ~OpenDTree() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
    virtual TreeNode<Entry>* search(TreeNode<Entry>* tree) override;
    virtual const Entry* search_and_cleanup(TreeNode<Entry>* tree) override;
    int count_branch(TreeNode<Entry>* tree);
};

#include "open_tree2.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
