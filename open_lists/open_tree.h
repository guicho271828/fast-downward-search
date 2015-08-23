/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_OPEN_TREE_H
#define OPEN_LISTS_OPEN_TREE_H

#include "abstract_tiebreaking_open_list.h"
#include "../per_state_information.h"
#include "treenode.h"

using namespace std;

template<class Entry>
using DB = PerStateInformation<TreeNode<Entry>*> ;

template<class Entry>
class OpenTree : public AbstractTieBreakingOpenList<Entry> {
    typedef vector<int> Key;
    map<Key,DB<Entry>*> trees;
    map<Key,int> counts; // for frontier_size.
    int size = 0;
    mt19937 gen = mt19937(1);

    PerStateInformation<TreeNode<Entry>*>* get_tree(Key &key);
    
protected:
    virtual void do_insertion(EvaluationContext &eval_context,
                              const Entry &entry) override;

public:
    explicit OpenTree(const Options &opts);
    virtual ~OpenTree() override = default;

    virtual Entry remove_min(vector<int> *key = 0) override;
    /* low priority */
    virtual bool empty() const override;
    virtual void clear() override;
    virtual int frontier_size() override;

    static OpenList<Entry> *_parse(OptionParser &parser);
};

#include "open_tree.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
