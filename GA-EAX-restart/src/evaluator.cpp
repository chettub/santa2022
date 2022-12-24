#include <bits/stdc++.h>
#include <iostream>

#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

using namespace std;
using ll = long long;

TEvaluator::TEvaluator() {
    fEdgeDis = NULL;
    fNearCity = NULL;
    Ncity = 0;
    fNearNumMax = 50;
}

TEvaluator::~TEvaluator() {
    for (ll i = 0; i < Ncity; ++i)
        delete[] fEdgeDis[i];
    delete[] fEdgeDis;

    for (ll i = 0; i < Ncity; ++i)
        delete[] fNearCity[i];
    delete[] fNearCity;

    delete[] x;
    delete[] y;
}

void TEvaluator::setInstance(char filename[]) {
    FILE* fp;
    ll n;
    char word[80], type[80];
    fp = fopen(filename, "r");

    /* read instance */
    while (1) {
        if (fscanf(fp, "%s", word) == EOF)
            break;
        if (strcmp(word, "DIMENSION") == 0) {
            fscanf(fp, "%s", word);
            fscanf(fp, "%lld", &Ncity);
        }
        if (strcmp(word, "EDGE_WEIGHT_TYPE") == 0) {
            fscanf(fp, "%s", word);
            fscanf(fp, "%s", type);
        }
        if (strcmp(word, "NODE_COORD_SECTION") == 0)
            break;
        // std::cout << word << std::endl;
        if (strcmp(word, "EDGE_WEIGHT_SECTION") == 0)
            break;
    }
    // std::cout << type << std::endl;
    // if (strcmp(word, "NODE_COORD_SECTION") != 0) {
    //     printf("Error in reading the instance\n");
    //     exit(0);
    // }
    x = new double[Ncity];
    y = new double[Ncity];
    ll* checkedN = new ll[Ncity];

    if (strcmp(type, "EXPLICIT") != 0) {
        for (ll i = 0; i < Ncity; ++i) {
            fscanf(fp, "%lld", &n);
            fscanf(fp, "%s", word);
            x[i] = atof(word);
            fscanf(fp, "%s", word);
            y[i] = atof(word);
        }
    }

    fEdgeDis = new ll*[Ncity];
    for (ll i = 0; i < Ncity; ++i)
        fEdgeDis[i] = new ll[Ncity];
    fNearCity = new ll*[Ncity];
    for (ll i = 0; i < Ncity; ++i)
        fNearCity[i] = new ll[fNearNumMax + 1];

    if (strcmp(type, "EUC_2D") == 0) {
        for (ll i = 0; i < Ncity; ++i)
            for (ll j = 0; j < Ncity; ++j)
                fEdgeDis[i][j] = (ll)(sqrt((x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j])) + 0.5);
    } else if (strcmp(type, "ATT") == 0) {
        for (ll i = 0; i < Ncity; ++i) {
            for (ll j = 0; j < Ncity; ++j) {
                double r = (sqrt(((x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j])) / 10.0));
                ll t = (ll)r;
                if ((double)t < r)
                    fEdgeDis[i][j] = t + 1;
                else
                    fEdgeDis[i][j] = t;
            }
        }
    } else if (strcmp(type, "CEIL_2D") == 0) {
        for (ll i = 0; i < Ncity; ++i)
            for (ll j = 0; j < Ncity; ++j)
                fEdgeDis[i][j] = (ll)ceil(sqrt((x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j])));
    } else if (strcmp(type, "EXPLICIT") == 0) {
        std::ifstream ifs(filename);
        std::string str;

        if (ifs.fail()) {
            std::cerr << "Failed to open file." << std::endl;
            exit(1);
        }
        do {
            getline(ifs, str);
        } while (str != "EDGE_WEIGHT_SECTION");

        for (ll i = 0; i < Ncity; ++i) {
            // std::cout << i << std::endl;
            for (ll j = 0; j < Ncity; j++) {
                ll dist;
                // fscanf(fp, "%s", word);
                // dist = atoll(word);

                ifs >> dist;
                fEdgeDis[j][i] = fEdgeDis[i][j] = (ll)dist;
                // std::cout << dist << std::endl;
            }
            // fscanf(fp, "%lld", &n);
            // fscanf(fp, "%s", word);
            // x[i] = atof(word);
            // fscanf(fp, "%s", word);
            // y[i] = atof(word);
        }
        // for (ll i = 0; i < Ncity; ++i) {
        //     for (ll j = 0; j < Ncity; j++) {
        //         std::cout << fEdgeDis[i][j] << " ";
        //     }
        //     std::cout << std::endl;
        // }
    } else {
        std::cout << type << std::endl;
        printf("EDGE_WEIGHT_TYPE is not supported\n");
        exit(1);
    }
    fclose(fp);
    ll ci, j1, j2, j3;
    ll cityNum = 0;
    ll minDis;
    for (ci = 0; ci < Ncity; ++ci) {
        for (j3 = 0; j3 < Ncity; ++j3)
            checkedN[j3] = 0;
        checkedN[ci] = 1;
        fNearCity[ci][0] = ci;
        for (j1 = 1; j1 <= fNearNumMax; ++j1) {
            minDis = 10000000000ll;
            for (j2 = 0; j2 < Ncity; ++j2) {
                if (fEdgeDis[ci][j2] <= minDis && checkedN[j2] == 0) {
                    cityNum = j2;
                    minDis = fEdgeDis[ci][j2];
                }
            }
            fNearCity[ci][j1] = cityNum;
            checkedN[cityNum] = 1;
        }
    }
}

void TEvaluator::doIt(TIndi& indi) {
    ll d = 0;
    for (ll i = 0; i < Ncity; ++i)
        d += fEdgeDis[i][indi.fLink[i][0]] + fEdgeDis[i][indi.fLink[i][1]];
    indi.fEvaluationValue = d / 2;
}

void TEvaluator::writeTo(FILE* fp, TIndi& indi) {
    Array = new ll[Ncity];
    ll curr = 0, st = 0, count = 0, pre = -1, next;
    while (1) {
        Array[count++] = curr + 1;
        if (count > Ncity) {
            printf("Invalid\n");
            return;
        }
        if (indi.fLink[curr][0] == pre)
            next = indi.fLink[curr][1];
        else
            next = indi.fLink[curr][0];

        pre = curr;
        curr = next;
        if (curr == st)
            break;
    }
    if (this->checkValid(Array, indi.fEvaluationValue) == false)
        printf("Individual is invalid \n");

    fprintf(fp, "%lld %lld\n", indi.fN, indi.fEvaluationValue);
    for (ll i = 0; i < indi.fN; ++i)
        fprintf(fp, "%lld ", Array[i]);
    fprintf(fp, "\n");
}

void TEvaluator::writeToStdout(TIndi& indi) {
    Array = new ll[Ncity];
    ll curr = 0, st = 0, count = 0, pre = -1, next;
    while (1) {
        Array[count++] = curr + 1;
        if (count > Ncity) {
            printf("Invalid\n");
            return;
        }
        if (indi.fLink[curr][0] == pre)
            next = indi.fLink[curr][1];
        else
            next = indi.fLink[curr][0];

        pre = curr;
        curr = next;
        if (curr == st)
            break;
    }
    if (this->checkValid(Array, indi.fEvaluationValue) == false)
        printf("Individual is invalid \n");

    printf("%lld %lld\n", indi.fN, indi.fEvaluationValue);
    for (ll i = 0; i < indi.fN; ++i)
        printf("%lld ", Array[i]);
    printf("\n");
}

bool TEvaluator::checkValid(ll* array, ll value) {
    ll* check = new ll[Ncity];
    for (ll i = 0; i < Ncity; ++i)
        check[i] = 0;
    for (ll i = 0; i < Ncity; ++i)
        ++check[array[i] - 1];
    for (ll i = 0; i < Ncity; ++i)
        if (check[i] != 1)
            return false;
    ll distance = 0;
    for (ll i = 0; i < Ncity - 1; ++i)
        distance += fEdgeDis[array[i] - 1][array[i + 1] - 1];

    distance += fEdgeDis[array[Ncity - 1] - 1][array[0] - 1];

    delete[] check;
    if (distance != value)
        return false;
    return true;
}
