#include <iostream>
#include <math.h>
#include <mutex>
#include <thread>
#include <vector>

#ifndef __ENVIRONMENT__
#include "environment.h"
#endif

std::mutex mtx;

using ll = long long;

extern ll gBestValue;
extern TIndi gBest;
extern ll duration;

void MakeRandSol(TEvaluator* eval, TIndi& indi);
void Make2optSol(TEvaluator* eval, TIndi& indi);

TEnvironment::TEnvironment() {
    fEvaluator = new TEvaluator();
}

TEnvironment::~TEnvironment() {
    delete[] fIndexForMating;
    delete[] tCurPop;
    delete fEvaluator;
    // delete tCross;
    delete[] tCross;

    ll N = fEvaluator->Ncity;
    for (ll i = 0; i < N; ++i)
        delete[] fEdgeFreq[i];
    delete[] fEdgeFreq;
}

void TEnvironment::define() {
    fEvaluator->setInstance(fFileNameTSP);
    ll N = fEvaluator->Ncity;
    fIndexForMating = new ll[Npop + 1];
    tCurPop = new TIndi[Npop];
    for (ll i = 0; i < Npop; ++i)
        tCurPop[i].define(N);

    gBestValue = -1;
    gBest.define(N);

    tBest.define(N);

    tCross = new TCross*[this->Nthread];
    for (int i = 0; i < Nthread; i++) {
        tCross[i] = new TCross(N);
        tCross[i]->eval = fEvaluator;
        tCross[i]->Npop = Npop;
    }

    // tCross = new TCross(N);
    // tCross->eval = fEvaluator;
    // tCross->Npop = Npop;

    tKopt = new TKopt*[this->Nthread];
    for (int i = 0; i < Nthread; i++) {
        tKopt[i] = new TKopt(N);
        tKopt[i]->eval = fEvaluator;
        tKopt[i]->setInvNearList();
    }

    fEdgeFreq = new int*[N];
    for (int i = 0; i < N; ++i)
        fEdgeFreq[i] = new int[N];
    this->fTimeStart = clock();

    kaizen.resize(Npop, true);
}

void TEnvironment::doIt() {
    this->initPop();
    this->init();
    this->getEdgeFreq();
    this->fTimeEnd = clock();
    duration = (ll)((double)(this->fTimeEnd - this->fTimeStart) / (double)CLOCKS_PER_SEC);

    fill(kaizen.begin(), kaizen.end(), true);

    while (duration < tmax) {
        this->setAverageBest();
        if (gBestValue == -1 || fBestValue < gBestValue) {
            gBestValue = fBestValue;
            gBest = tBest;
            // printf("find better solution %lld\n", gBestValue);
            if (gBestValue <= this->optimum) {
                printf("Find optimal solution %lld, exit\n", gBestValue);
                this->terminate = true;
                break;
            }
        }
        if (fCurNumOfGen % 50 == 0) {
            this->fTimeEnd = clock();
            duration = (ll)((double)(this->fTimeEnd - this->fTimeStart) / (double)CLOCKS_PER_SEC);
            printf("%lld:\t%lld:\t%lld\t%lf\n", fCurNumOfGen, duration, fBestValue, fAverageValue);
            this->writeBest();
            // record time every 50 gens
            if (duration >= tmax)
                break;
        }

        if (this->terminationCondition())
            break;

        this->selectForMating();

        // high cpu usage parallelization
        // has a bug in it and does not work well
        // assert(Npop >= Nthread * 2);
        // vector<thread> threads(Nthread);
        // vector<bool> flagfinished(Nthread, false);
        // vector<int> lastindex(Nthread);
        // for (int i = 0; i < Nthread; i++) {
        //     int s1 = i;
        //     int s2 = i + Nthread;
        //     thread th([&](int s1, int s2, int i) {
        //         lastindex[i] = s2;
        //         this->generateKids(s1, s2, i);
        //         flagfinished[i] = true;
        //     },
        //         s1, s2, i);
        //     threads[i] = move(th);
        // }
        // set<int> sleft;
        // for (int s = Nthread * 2; s < Npop; s++) {
        //     sleft.insert(s);
        // }
        // for (int _ = 0; _ < Npop - Nthread; _++) {
        //     bool breakflag = false;
        //     while (true) {
        //         for (int i = 0; i < Nthread; i++) {
        //             if (flagfinished[i] == true && lastindex[i] >= Nthread) {
        //                 threads[i].join();
        //                 flagfinished[i] = false;
        //                 breakflag = true;

        //                 int s1 = lastindex[i];
        //                 int s2;
        //                 if (sleft.empty())
        //                     s2 = i;
        //                 else {
        //                     s2 = *sleft.begin();
        //                     sleft.erase(s2);
        //                 }
        //                 cout << s1 << " " << s2 << endl;
        //                 thread th([&](int s1, int s2, int i) {
        //                     lastindex[i] = s2;
        //                     this->generateKids(s1, s2, i);
        //                     flagfinished[i] = true;
        //                 },
        //                     s1, s2, i);
        //                 threads[i] = move(th);
        //                 break;
        //             }
        //         }
        //         if (breakflag)
        //             break;
        //     }
        // }
        // for (auto& th : threads)
        //     th.join();

        // naive parallel
        assert(Npop % Nthread == 0);
        vector<thread> threads;
        for (int i = 0; i < Nthread; i++) {
            // int s = j_thread * Nthread + i;
            thread th([&](int i) {
                for (ll j_thread = 0; j_thread < Npop / Nthread / 2; j_thread++) {
                    int s = j_thread + i * Npop / Nthread;
                    this->generateKids(s, i);
                }
            },
                i);
            threads.push_back(move(th));
        }
        for (auto& th : threads)
            th.join();
        threads.clear();
        for (int i = 0; i < Nthread; i++) {
            // int s = j_thread * Nthread + i;
            thread th([&](int i) {
                for (ll j_thread = Npop / Nthread / 2; j_thread < Npop / Nthread; j_thread++) {
                    int s = j_thread + i * Npop / Nthread;
                    this->generateKids(s, i);
                }
            },
                i);
            // th.join();
            threads.push_back(move(th));
            // threads[0].join();
        }
        for (auto& th : threads)
            th.join();

        ++fCurNumOfGen;
    }

    if (duration >= tmax)
        this->terminate = true;
}

void TEnvironment::init() {
    fAccumurateNumCh = 0;
    fCurNumOfGen = 0;
    fStagBest = 0;
    fMaxStagBest = 0;
    fStage = 1;    /* Stage I */
    fFlagC[0] = 4; /* Diversity preservation: 1:Greedy, 2:--- , 3:Distance, 4:Entropy (see Section 4) */
    fFlagC[1] = 1; /* Eset Type: 1:Single-AB, 2:Block2 (see Section 3) */
}

bool TEnvironment::terminationCondition() {
    if (fAverageValue - fBestValue < 0.001)
        return true;

    int Nkaizen = 0;
    for (int i = 0; i < Npop; i++) {
        Nkaizen += kaizen[i];
    }

    if ((Npop - Nkaizen) > int(Npop * 0.99)) {
        if (fStage == 1) {
            fFlagC[1] = 2;
            fStage = 2;
            fCurNumOfGen1 = fCurNumOfGen;
            std::cout << "Start Stage 2" << std::endl;
            return false;
        } else {
            ll dGen = fCurNumOfGen - fCurNumOfGen1;
            if (dGen <= 200)
                return false;
        }
        return true;
    } else
        return false;


    // OLD
    if (fStage == 1) /* Stage I */
    {
        /* 1500/N_ch (See Section 2.2) */
        if (fStagBest == ll(1500 / Nch) && fMaxStagBest == 0) {
            /* fMaxStagBest = G/10 (See Section 2.2) */
            fMaxStagBest = ll(fCurNumOfGen / 10);
        }
        /* Terminate Stage I (proceed to Stage II) */
        else if (fMaxStagBest != 0 && fMaxStagBest <= fStagBest) {
            fStagBest = 0;
            fMaxStagBest = 0;
            fCurNumOfGen1 = fCurNumOfGen;
            fFlagC[1] = 2;
            fStage = 2;
            std::cout << "Start Stage 2" << std::endl;
        }
        return false;
    }
    if (fStage == 2) /* Stage II */
    {
        /* 1500/N_ch */
        if (fStagBest == ll(1500 / Nch) && fMaxStagBest == 0) {
            /* fMaxStagBest = G/10 (See Section 2.2) */
            fMaxStagBest = ll((fCurNumOfGen - fCurNumOfGen1) / 10);
            fMaxStagBest = max(fMaxStagBest, 50ll);
        }
        /* Terminate Stage II and GA */
        // else if (fMaxStagBest != 0 && fMaxStagBest <= fStagBest) {
        else if (fMaxStagBest != 0 && fMaxStagBest * 4 <= fStagBest) {
            return true;
        }
        return false;
    }

    return true;
}

void TEnvironment::setAverageBest() {
    ll stockBest = tBest.fEvaluationValue;
    fAverageValue = 0.0;
    fBestIndex = 0;
    fBestValue = tCurPop[0].fEvaluationValue;
    for (ll i = 0; i < Npop; ++i) {
        fAverageValue += tCurPop[i].fEvaluationValue;
        if (tCurPop[i].fEvaluationValue < fBestValue) {
            fBestIndex = i;
            fBestValue = tCurPop[i].fEvaluationValue;
        }
    }
    tBest = tCurPop[fBestIndex];
    fAverageValue /= (double)Npop;
    if (tBest.fEvaluationValue < stockBest) {
        fStagBest = 0;
        fBestNumOfGen = fCurNumOfGen;
        fBestAccumeratedNumCh = fAccumurateNumCh;
    } else
        ++fStagBest;
}

void TEnvironment::initPop() {
    // naive parallel
    assert(Npop % Nthread == 0);
    for (ll j_thread = 0; j_thread < Npop / Nthread; j_thread++) {
        vector<thread> threads;
        for (int i = 0; i < Nthread; i++) {
            int s = j_thread + i * Npop / Nthread;
            thread th([&](int s, int i) {
                tKopt[i]->makeRandSol(tCurPop[s]); /* Make a random tour */
                tKopt[i]->doIt(tCurPop[s]);        /* Apply the local search with the 2-opt neighborhood */
            },
                s, i);
            threads.push_back(move(th));
        }
        for (auto& th : threads)
            th.join();
    }

    // original
    // for (ll i = 0; i < Npop; ++i) {
    //     tKopt->makeRandSol(tCurPop[i]); /* Make a random tour */
    //     tKopt->doIt(tCurPop[i]);        /* Apply the local search with the 2-opt neighborhood */
    // }
}

void TEnvironment::selectForMating() {
    /* fIndexForMating[] <-- a random permutation of 0, ..., fNumOfPop-1 */
    tRand->permutation(fIndexForMating, Npop, Npop);
    fIndexForMating[Npop] = fIndexForMating[0];
}

void TEnvironment::generateKids(ll s, int i) {
    /* Note: tCurPop[fIndexForMating[s]] is replaced with a best offspring solutions in tCorss->DoIt().
     fEegeFreq[][] is also updated there. */
    // update s1
    // tCross[i]->setParents(tCurPop[fIndexForMating[s1]], tCurPop[fIndexForMating[s2]], fFlagC, Nch);
    // tCross[i]->doIt(tCurPop[fIndexForMating[s1]], tCurPop[fIndexForMating[s2]], Nch, 1, fFlagC, fEdgeFreq);
    // fAccumurateNumCh += tCross[i]->fNumOfGeneratedCh;
    ll costbef = tCurPop[fIndexForMating[s]].fEvaluationValue;
    tCross[i]->setParents(tCurPop[fIndexForMating[s]], tCurPop[fIndexForMating[s + 1]], fFlagC, Nch);
    tCross[i]->doIt(tCurPop[fIndexForMating[s]], tCurPop[fIndexForMating[s + 1]], Nch, 1, fFlagC, fEdgeFreq);
    ll costnew = tCurPop[fIndexForMating[s]].fEvaluationValue;
    kaizen[fIndexForMating[s]] = costnew < costbef;
    fAccumurateNumCh += tCross[i]->fNumOfGeneratedCh;
}

void TEnvironment::getEdgeFreq() {
    ll k0, k1, N = fEvaluator->Ncity;
    for (ll j1 = 0; j1 < N; ++j1)
        for (ll j2 = 0; j2 < N; ++j2)
            fEdgeFreq[j1][j2] = 0;

    for (ll i = 0; i < Npop; ++i)
        for (ll j = 0; j < N; ++j) {
            k0 = tCurPop[i].fLink[j][0];
            k1 = tCurPop[i].fLink[j][1];
            ++fEdgeFreq[j][k0];
            ++fEdgeFreq[j][k1];
        }
}

void TEnvironment::printOn() {
    printf("Total time: %lld\n", duration);
    printf("bestval = %lld, optimum = %lld \n", gBestValue, this->optimum);
    fEvaluator->writeToStdout(gBest);
    if (gBestValue != -1 && gBestValue <= this->optimum)
        printf("Successful\n");
    else
        printf("Unsuccessful\n");
    fflush(stdout);
}

void TEnvironment::writeBest() {
    FILE* fp;
    char filename[80];

    sprintf(filename, "bestSolution.txt");
    fp = fopen(filename, "a");
    fEvaluator->writeTo(fp, gBest);
    fclose(fp);
}
