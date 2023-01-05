
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef __KOPT__
#define __KOPT__

#ifndef __RAND__
#include "randomize.h"
#endif

#ifndef __Sort__
#include "sort.h"
#endif

#ifndef __INDI__
#include "indi.h"
#endif

#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

using ll = long long;

class TKopt {
public:
    TKopt(ll N);
    ~TKopt();
    void setInvNearList();
    void transIndiToTree(TIndi& indi);
    void transTreeToIndi(TIndi& indi);
    void doIt(TIndi& tIndi); /* Apply a local search with the 2-opt neighborhood */

    ll getNext(ll t);
    ll getPrev(ll t);
    ll turn(ll& orient);

    void sub();
    void incrementImp(ll flagRev);
    void combineSeg(ll segL, ll segS);

    void checkDetail();
    void checkValid();
    void swap(ll& x, ll& y);
    void makeRandSol(TIndi& indi); /* Set a random tour */

    TEvaluator* eval;

private:
    ll fN;
    ll fFixNumOfSeg;
    ll fNumOfSeg;
    ll fFlagRev;
    int128 fTourLength;

    ll** fLink;
    ll** fLinkSeg;
    ll** fCitySeg;
    ll** fInvNearList;

    ll* fT;
    ll* fB;
    ll* fSegCity;
    ll* fOrdCity;
    ll* fOrdSeg;
    ll* fOrient;
    ll* fSizeSeg;
    ll* fActiveV;
    ll* fNumOfINL;
    ll* fArray;
    ll* fCheckN;
    ll* fGene;
};

#endif
