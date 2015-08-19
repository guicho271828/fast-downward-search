/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_UCB_LIST_H
#define OPEN_LISTS_UCB_LIST_H

#include "bandit_list.h"


template<class Reward, class Entry>
class UCBPlateau : public Plateau<Reward,Entry,UCB> {
public:
    UCBPlateau(Reward _k) :
        Plateau<Reward,Entry,UCB>() {
        // the name k was not successfully resolved... pity
        this->k=_k;
    };
    ~UCBPlateau(){};
};

template<class Entry>
class UCBOpenList : public BanditOpenList<Entry,UCB> {
    typedef typename BanditOpenList<Entry,UCB>::P P;
    double k;
protected:
    P* make_plateau(){
        return new UCBPlateau<double,Entry>(k);
    };
public:
    explicit UCBOpenList(const Options &opts);
    ~UCBOpenList() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
};





template<class Reward, class Entry>
class RandomPlateau : public Plateau<Reward,Entry,RandomBandit> {
public:
    RandomPlateau(Reward _k) :
        Plateau<Reward,Entry,RandomBandit>() {
        // the name k was not successfully resolved... pity
        this->k=_k;
    };
    ~RandomPlateau(){};
};

template<class Entry>
class RandomDepthOpenList : public BanditOpenList<Entry,RandomBandit> {
    typedef typename BanditOpenList<Entry,RandomBandit>::P P;
    double k;
protected:
    P* make_plateau(){
        return new RandomPlateau<double,Entry>(k);
    };
public:
    explicit RandomDepthOpenList(const Options &opts);
    ~RandomDepthOpenList() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
};


#include "ucb_list.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
