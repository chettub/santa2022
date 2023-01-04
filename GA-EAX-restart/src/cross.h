#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifndef __Cross__
#define __Cross__

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

class TCross {
public:
    TCross(ll N);
    ~TCross();
    void doIt(TIndi& tKid, TIndi& tPa2, ll numOfKids, ll flagP, ll flagC[10], int** fEdgeFreq); /* Main procedure of EAX */
    void setParents(const TIndi& tPa1, const TIndi& tPa2, ll flagC[10], ll numOfKids);          /* Set information of the parent tours */
    void setABcycle(const TIndi& parent1, const TIndi& parent2, ll flagC[10], ll numOfKids);    /* Step 2 of EAX */

    void swap(ll& x, ll& y);                        /* Swap */
    void formABcycle();                             /* Store an AB-cycle found */
    void changeSol(TIndi& tKid, ll ABnum, ll type); /* Apply an AB-cycle to an llermediate solution */

    void makeCompleteSol(TIndi& tKid); /* Step 5 of EAX */
    void makeUnit();                   /* Step 5-1 of EAX */
    void backToPa1(TIndi& tKid);       /* Undo the parent p_A */
    void goToBest(TIndi& tKid);        /* Modify tKid to the best offspring solution */

    void incrementEdgeFreq(int** fEdgeFreq); /* Increment fEdgeFreq[][] */
    ll calAdpLoss(int** fEdgeFreq);          /* Compute the difference in the averate distance */
    double calEntLoss(int** fEdgeFreq);      /* Compute the difference in the edge entropy */

    /* Block2 */
    void setWeight(const TIndi& parent1, const TIndi& parent2);
    ll calCNaive();
    void searchEset(ll num);
    void addAB(ll num);
    void deleteAB(ll num);

    ll fNumOfGeneratedCh;
    TEvaluator* eval;
    ll Npop;

private:
    TIndi tBestTmp;
    ll fFlagImp;
    ll fN;
    ll r;
    ll exam;
    ll examFlag;
    ll flagSt;
    ll flagCycle;
    ll prType;
    ll chDis;
    ll koritsuMany;
    ll bunkiMany;
    ll st;
    ll ci;
    ll pr;
    ll stock;
    ll stAppear;
    ll fEvalType;
    ll fEsetType;
    ll fNumOfABcycleInESet;
    ll fNumOfABcycle;
    ll fPosiCurr;
    ll fMaxNumOfABcycle;

    ll* koritsu;
    ll* bunki;
    ll* koriInv;
    ll* bunInv;
    ll* checkKoritsu;
    ll* fRoute;
    ll* fPermu;
    ll* fC;
    ll* fJun;
    ll* fOrd1;
    ll* fOrd2;

    ll** nearData;
    int** fABcycle;

    // Speed Up Start
    ll fNumOfUnit;
    ll fNumOfSeg;
    ll fNumOfSPL;
    ll fNumOfElementInCU;
    ll fNumOfSegForCenter;
    int128 fGainModi;
    ll fNumOfModiEdge;
    ll fNumOfBestModiEdge;
    ll fNumOfAppliedCycle;
    ll fNumOfBestAppliedCycle;

    ll* fOrder;
    ll* fInv;
    ll* fSegUnit;
    ll* fSegPosiList;
    ll* LinkAPosi;
    ll* fPosiSeg;
    ll* fNumOfElementInUnit;
    ll* fCenterUnit;
    ll* fListOfCenterUnit;
    ll* fSegForCenter;
    int128* fGainAB;
    ll* fAppliedCylce;
    ll* fBestAppliedCylce;

    ll** fSegment;
    ll** LinkBPosi;
    ll** fModiEdge;
    ll** fBestModiEdge;
    // Speed Up End

    // Block2
    ll fNumOfUsedAB;
    ll fNumC;
    ll fNumE;
    ll fTmax;
    ll fMaxStag;
    ll fNumOfABcycleInEset;
    ll fDisAB;
    ll fBestNumC;
    ll fBestNumE;

    ll* fNumOfElementINAB;
    ll* fWeightSR;
    ll* fWeightC;
    ll* fUsedAB;
    ll* fMovedAB;
    ll* fABcycleInEset;

    ll** fInEffectNode;
    ll** fWeightRR;
};

#endif
