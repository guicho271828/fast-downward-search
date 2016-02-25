#ifndef PLUGIN_H
#define PLUGIN_H

#include "open_lists/alternation_open_list.h"
#include "open_lists/bucket_open_list.h"
// #include "open_lists/pareto_open_list.h"
#include "open_lists/standard_scalar_open_list.h"
#include "open_lists/tiebreaking_open_list.h"
#include "open_lists/open_tree.h"
// #include "open_lists/open_tree2.h"
#include "open_lists/bandit_list.h"
#include "open_lists/ucb_list.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

template <class T>
class Plugin {
public:
    Plugin(const std::string &key, typename Registry<T *>::Factory factory) {
        Registry<T *>::instance()->register_object(key, factory);
    }
    ~Plugin() = default;
    Plugin(const Plugin<T> &other) = delete;
};


// TODO: This class will replace Plugin once we no longer need to support raw pointers.
template <class T>
class PluginShared {
public:
    PluginShared(const std::string &key, typename Registry<std::shared_ptr<T> >::Factory factory) {
        Registry<std::shared_ptr<T> >::instance()->register_object(key, factory);
    }
    ~PluginShared() = default;
    PluginShared(const PluginShared<T> &other) = delete;
};

template <class Entry>
class Plugin<OpenList<Entry > > {
    Plugin(const Plugin<OpenList<Entry > > &copy);
public:
    ~Plugin();

    static void register_open_lists() {
        Registry<OpenList<Entry > *>::instance()->register_object(
            "single", StandardScalarOpenList<Entry>::_parse);
        Registry<OpenList<Entry > *>::instance()->register_object(
            "single_buckets", BucketOpenList<Entry>::_parse);
        Registry<OpenList<Entry > *>::instance()->register_object(
            "tiebreaking", TieBreakingOpenList<Entry>::_parse);
        Registry<OpenList<Entry > *>::instance()->register_object(
            "alt", AlternationOpenList<Entry>::_parse);
        /* Registry<OpenList<Entry > *>::instance()->register_object( */
        /*     "pareto", ParetoOpenList<Entry>::_parse); */
        Registry<OpenList<Entry > *>::instance()->register_object(
            "tree", OpenTree<Entry>::_parse);
        /* Registry<OpenList<Entry > *>::instance()->register_object( */
        /*     "dtree", OpenDTree<Entry>::_parse); */
        Registry<OpenList<Entry > *>::instance()->register_object(
            "ucb", UCBOpenList<Entry>::_parse);
        Registry<OpenList<Entry > *>::instance()->register_object(
            "rd", RandomDepthOpenList<Entry>::_parse);
        Registry<OpenList<Entry > *>::instance()->register_object(
            "fd", FirstDepthOpenList<Entry>::_parse);
        Registry<OpenList<Entry > *>::instance()->register_object(
            "ld", LastDepthOpenList<Entry>::_parse);
        Registry<OpenList<Entry > *>::instance()->register_object(
            "loop", LoopingDepthOpenList<Entry>::_parse);
    }
};

#endif
