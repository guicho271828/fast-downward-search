/* -*- mode : c++ -*- */
#ifndef OPEN_LISTS_UCB_LIST_H
#define OPEN_LISTS_UCB_LIST_H

#include "bandit_list.h"


template<class Reward, class Entry>
class UCBPlateau : public Plateau<Reward,Entry,UCB> {
public:
    UCBPlateau(Reward _k, int _queue, int _seed3) :
        Plateau<Reward,Entry,UCB>(_queue,_seed3) {
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
        return new UCBPlateau<double,Entry>(k,this->queue,this->seed3);
    };
public:
    explicit UCBOpenList(const Options &opts);
    ~UCBOpenList() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
};





template<class Reward, class Entry>
class RandomPlateau : public Plateau<Reward,Entry,RandomBandit> {
public:
    RandomPlateau(int _queue, int _seed2, int _seed3) :
        Plateau<Reward,Entry,RandomBandit>(_queue, _seed3) {
        // inherited from RandomBandit
        this->rng(_seed2);
    };
    ~RandomPlateau(){};
};

template<class Entry>
class RandomDepthOpenList : public BanditOpenList<Entry,RandomBandit> {
    typedef typename BanditOpenList<Entry,RandomBandit>::P P;
    int seed2;
protected:
    P* make_plateau(){
        // this->queue : inherited from AbstractTieBreakingOpenList
        return new RandomPlateau<double,Entry>(this->queue,seed2,this->seed3);
    };
public:
    explicit RandomDepthOpenList(const Options &opts);
    ~RandomDepthOpenList() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
};




template<class Reward, class Entry>
class FirstPlateau : public Plateau<Reward,Entry,FirstBandit> {
public:
    FirstPlateau(int _queue, int _seed3) :
        Plateau<Reward,Entry,FirstBandit>(_queue, _seed3) {
    };
    ~FirstPlateau(){};
};

template<class Entry>
class FirstDepthOpenList : public BanditOpenList<Entry,FirstBandit> {
    typedef typename BanditOpenList<Entry,FirstBandit>::P P;
protected:
    P* make_plateau(){
        return new FirstPlateau<double,Entry>(this->queue, this->seed3);
    };
public:
    explicit FirstDepthOpenList(const Options &opts);
    ~FirstDepthOpenList() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
};




template<class Reward, class Entry>
class LastPlateau : public Plateau<Reward,Entry,LastBandit> {
public:
    LastPlateau(int _queue, int _seed3) :
        Plateau<Reward,Entry,LastBandit>(_queue, _seed3) {
    };
    ~LastPlateau(){};
};

template<class Entry>
class LastDepthOpenList : public BanditOpenList<Entry,LastBandit> {
    typedef typename BanditOpenList<Entry,LastBandit>::P P;
protected:
    P* make_plateau(){
        return new LastPlateau<double,Entry>(this->queue, this->seed3);
    };
public:
    explicit LastDepthOpenList(const Options &opts);
    ~LastDepthOpenList() override = default;
    static OpenList<Entry> *_parse(OptionParser &parser);
};




#include "ucb_list.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
