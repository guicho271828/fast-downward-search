/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_UCB_LIST_H
#define OPEN_LISTS_UCB_LIST_H

#include "bandit_list.h"


template<class Reward, class Entry>
class UCBPlateau : public Plateau<Reward,Entry,UCB> {
public:
    UCBPlateau(Reward _k, int _queue) :
        Plateau<Reward,Entry,UCB>() {
        // the name k was not successfully resolved... pity
        this->k=_k;
        this->queue=_queue;
    };
    ~UCBPlateau(){};
};

template<class Entry>
class UCBOpenList : public BanditOpenList<Entry,UCB> {
    typedef typename BanditOpenList<Entry,UCB>::P P;
    double k;
protected:
    P* make_plateau(){
        return new UCBPlateau<double,Entry>(this->k,this->queue);
    };
public:
    explicit UCBOpenList(const Options &opts);
    ~UCBOpenList() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
};





template<class Reward, class Entry>
class RandomPlateau : public Plateau<Reward,Entry,RandomBandit> {
public:
    RandomPlateau(int _queue) : Plateau<Reward,Entry,RandomBandit>() {
        this->queue=_queue;
    };
    ~RandomPlateau(){};
};

template<class Entry>
class RandomDepthOpenList : public BanditOpenList<Entry,RandomBandit> {
    typedef typename BanditOpenList<Entry,RandomBandit>::P P;
protected:
    P* make_plateau(){
        return new RandomPlateau<double,Entry>(this->queue);
    };
public:
    explicit RandomDepthOpenList(const Options &opts);
    ~RandomDepthOpenList() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
};


#include "ucb_list.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
