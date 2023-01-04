/*
 * main.cpp
 *   created on: Oct 6, 2020
 *       author: shengcailiu (liusccc@gmail.com)
 */

#ifndef __ENVIRONMENT__
#include "environment.h"
#endif

#include "int128.h"

#include <csignal>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

using ll = long long;
using int128 = __int128;

int128 gBestValue = -1;  // global best value
TIndi gBest;             // global best solution
ll optimum;              // optimum cost
ll duration;             // used time
                         //
TEnvironment* gEnv = new TEnvironment();

void signalHandler(int signum) {
    cout << endl
         << "Signal (" << signum << ") received.\n";
    cout << endl;

    printf("Total time: %lld\n", duration);
    printf("bestval = %s, optimum = %lld \n", to_string_int128(gBestValue).c_str(), optimum);

    gEnv->writeAll("Interrupted_" + to_string(duration) + ".txt", 0);

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
    printf("%lld %s\n", gBest.fN, to_string_int128(gBest.fEvaluationValue).c_str());
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

    ll NRunmax = 1000000;

    // ./GA-EAX-restart tsp_file NPOP NCH optimum tmax
    // default: 100, 30, -1(unknown optimum), 3600
    if (!(9 <= argc && argc <= 13)) {
        cout << "./GA-EAX-restart tsp_file NPOP NCH optimum tmax_or_-runmax seed \
gain_constraint [initial_route_file] [starting_stage] [minimum_steps] \
[minimum_improve_population_ratio]\n";
        exit(-1);
    }
    gEnv->fFileNameTSP = argv[1];
    gEnv->Npop = atoi(argv[2]);
    gEnv->Nch = atoi(argv[3]);
    optimum = gEnv->optimum = atoi(argv[4]);
    if (atoll(argv[5]) >= 0)
        gEnv->tmax = atoll(argv[5]);
    else {
        gEnv->tmax = 10000000ll;
        NRunmax = -atoll(argv[5]);
    }
    InitURandom(atoi(argv[6]));
    gEnv->terminate = false;
    gEnv->Nthread = atoi(argv[7]);
    gEnv->GainConstraint = atof(argv[8]);
    if (argc >= 10)
        gEnv->initial_route_path = argv[9];
    if (argc >= 11)
        gEnv->initial_stage = atoi(argv[10]);
    else
        gEnv->initial_stage = 1;
    if (argc >= 12)
        gEnv->minimumSteps = atoll(argv[11]);
    else
        gEnv->minimumSteps = 200;
    if (argc >= 13)
        gEnv->minimumImprovePopulationRatio = atof(argv[12]);
    else
        gEnv->minimumImprovePopulationRatio = 0.01;

    cout << "Initialization ..." << endl;
    gEnv->define();
    for (ll n = 0; n < NRunmax; ++n) {
        printf("Run %lld\n", n);
        gEnv->Nrun = n;
        gEnv->doIt();
        gEnv->printOn();
        if (gEnv->terminate)
            break;
    }

    return 0;
}
