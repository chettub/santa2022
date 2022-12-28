/*
 * main.cpp
 *   created on: Oct 6, 2020
 *       author: shengcailiu (liusccc@gmail.com)
 */

#ifndef __ENVIRONMENT__
#include "environment.h"
#endif

#include <csignal>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

using ll = long long;

ll gBestValue = -1;  // global best value
TIndi gBest;         // global best solution
ll optimum;          // optimum cost
ll duration;         // used time
                     //
TEnvironment* gEnv = new TEnvironment();

void signalHandler(int signum) {
    cout << endl
         << "Signal (" << signum << ") received.\n";
    cout << endl;

    printf("Total time: %lld\n", duration);
    printf("bestval = %lld, optimum = %lld \n", gBestValue, optimum);

    gEnv->writeAll("Interrupted_" + to_string(duration) + ".txt");

    ll Ncity = gBest.fN;
    ll* Array = new ll[Ncity];
    ll curr = 0, st = 0, count = 0, pre = -1, next;
    while (1) {
        Array[count++] = curr + 1;
        if (count > Ncity) {
            printf("Invalid\n");
            return;
        }
        if (gBest.fLink[curr][0] == pre)
            next = gBest.fLink[curr][1];
        else
            next = gBest.fLink[curr][0];

        pre = curr;
        curr = next;
        if (curr == st)
            break;
    }
    printf("%lld %lld\n", gBest.fN, gBest.fEvaluationValue);
    // for (ll i = 0; i < gBest.fN; ++i)
    //     printf("%lld ", Array[i]);
    // printf("\n");

    if (gBestValue != -1 && gBestValue <= optimum)
        printf("Successful\n");
    else
        printf("Unsuccessful\n");
    fflush(stdout);

    exit(signum);
}

signed main(int argc, char* argv[]) {
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    gEnv->fFileNameTSP = (char*)malloc(100);

    // ./GA-EAX-restart tsp_file NPOP NCH optimum tmax
    // default: 100, 30, -1(unknown optimum), 3600
    if (argc != 8 && argc != 9) {
        cout << "./GA-EAX-restart tsp_file NPOP NCH optimum tmax seed Nthread [gain_constraint]\n";
        exit(-1);
    }
    gEnv->fFileNameTSP = argv[1];
    gEnv->Npop = atoi(argv[2]);
    gEnv->Nch = atoi(argv[3]);
    optimum = gEnv->optimum = atoi(argv[4]);
    gEnv->tmax = atoll(argv[5]);
    InitURandom(atoi(argv[6]));
    gEnv->terminate = false;
    gEnv->Nthread = atoi(argv[7]);
    if (argc >= 9)
        gEnv->GainConstraint = atof(argv[8]);
    else
        gEnv->GainConstraint = 0.0;

    cout << "Initialization ..." << endl;
    gEnv->define();
    for (ll n = 0; n < 1000000; ++n) {
        printf("Run %lld\n", n);
        gEnv->doIt();
        if (gEnv->terminate)
            break;
    }
    gEnv->printOn();

    return 0;
}
