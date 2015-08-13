#ifndef OPEN_LISTS_OPEN_TREE_H
#define OPEN_LISTS_OPEN_TREE_H

#include "abstract_tiebreaking_open_list.h"
#include "../per_state_information.h"

using namespace std;

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
    void set_entry(Entry* entry2){ assert(!entry); entry = entry2; };
    void delete_entry(){
        if (entry){
            delete entry;
            entry = nullptr;
        }
    };
    set<TreeNode*> children;
    const GlobalState state;
    void dump(int level = 0){
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
};

template<class Entry>
using DB = PerStateInformation<TreeNode<Entry>*> ;

template<class Entry>
void cleanup(TreeNode<Entry>* tree, DB<Entry>* db);

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
    virtual TreeNode<Entry>* search(TreeNode<Entry>* tree);
    virtual const Entry search_and_cleanup(TreeNode<Entry>* tree, DB<Entry>* db);

    static OpenList<Entry> *_parse(OptionParser &parser);
};

#include "open_tree.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
