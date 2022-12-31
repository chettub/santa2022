#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>


#ifndef __ENVIRONMENT__
#define __ENVIRONMENT__

#ifndef __INDI__
#include "indi.h"
#endif

#ifndef __RAND__
#include "randomize.h"
#endif

#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

#ifndef __Cross__
#include "cross.h"
#endif

#ifndef __KOPT__
#include "kopt.h"
#endif


using ll = long long;

class TEnvironment {
public:
    TEnvironment();
    ~TEnvironment();

    void define();               /* Define the variables */
    void doIt();                 /* Main procedure of the GA */
    void init();                 /* Initialization of the GA */
    bool terminationCondition(); /* Decide whether to proceed to next stage (or treminate the GA) */
    void setAverageBest();       /* Compute average and best tour lengths of the population */

    void initPop();                 /* Create an initial population */
    void selectForMating();         /* Determine a set of pairs of parents at each generation */
    void generateKids(ll s, int i); /* Generate offspring solutions from a selected pair of parents. Selection for survival is also performed here. */
    void getEdgeFreq();             /* Compute the frequency of the edges of the population */

    void printOn();                                          /* Display and write summary of results */
    void writeBest();                                        /* Write the best tour */
    void writeAll(const string path, bool flag_final_route); /* Write all tours */

    void readPop(const string path); /* read population */

    TEvaluator* fEvaluator; /* Distance of the edges */
    TCross** tCross;        /* Eede assembly crossover */
    TKopt** tKopt;          /* Local search with the 2-opt neighborhood */
    char* fFileNameTSP;     /* File name of an TSP instance */
    ll optimum;             /* best known optimum cost */
    ll tmax;                /* maximum running time in seconds*/
    bool terminate;         /* if terminate immediately */

    ll Npop;                      /* Number of population members (N_pop in the paper) */
    ll Nch;                       /* Number of offspring solutions (N_ch in the paper) */
    TIndi* tCurPop;               /* Current population members */
    TIndi tBest;                  /* Best solution in the current population */
    ll fCurNumOfGen;              /* The current number of generations */
    volatile ll fAccumurateNumCh; /* The accumulated number of offspring solutions */

    ll fBestNumOfGen;         /* The number of generations at which the current best solution was found */
    ll fBestAccumeratedNumCh; /* The accumulated number of offspring solutions at which the current best solution was found */
    int** fEdgeFreq;          /* The frequency of the edges of the population */
    double fAverageValue;     /* The average tour lengths of the population */
    ll fBestValue;            /* The tour lenght of the best tour in the population */
    ll fBestIndex;            /* Index of the best population member */

    ll* fIndexForMating; /* Mating list (r[] in the paper) */
    ll fStagBest;        /* The number of generations during which no improvement is found in the best tour */
    ll fFlagC[10];       /* Specify configurations of EAX and selection strategy */
    ll fStage;           /* Current stage */
    ll fMaxStagBest;     /* If fStagBest = fMaxStagBest, proceed to the next stage */
    ll fCurNumOfGen1;    /* Number of generations at which Stage I is terminated */

    clock_t fTimeStart, fTimeInit, fTimeEnd; /* Use them to measure the execution time */

    ll Nthread;

    vector<bool> kaizen;
    double GainConstraint;
    string initial_route_path;
    int Npopnow; /* Number of population members now */
    int initial_stage;
    int Nrun;
};

#endif