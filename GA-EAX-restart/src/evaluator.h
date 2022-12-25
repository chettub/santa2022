#include <assert.h>
#include <bits/stdc++.h>
#include <string.h>
#ifndef __EVALUATOR__
#define __EVALUATOR__

#ifndef __INDI__
#include "indi.h"
#endif

using namespace std;

using ll = long long;

int encode(int x, int y);
pair<int, int> decode(int i);
int encodesmall(int x, int y);
pair<int, int> decodesmall(int i);
bool checkin(int x, int y);

class TEvaluator {
public:
    TEvaluator();
    ~TEvaluator();
    void setInstance(char filename[]);    /* Set the instance */
    void doIt(TIndi& indi);               /* Set the value of indi.fEvaluationValue */
    void writeTo(FILE* fp, TIndi& indi);  /* Write an tour to a file*/
    void writeToStdout(TIndi& indi);      /* Write a tour to stdout */
    bool checkValid(ll* array, ll value); /* Check an tour */
    ll funcEdgeDis(int i, int j);         /* funcEdgeDis[i][j]: distance between i and j */

    ll fNearNumMax; /* Maximum number of k (see below) */
    ll** fNearCity; /* NearCity[i][k]: k-th nearest city from */
    ll** fEdgeDis;  /* EdgeDis[i][j]: distance between i and j */
    ll Ncity;       /* Number of cities */
    double* x;      /* x[i]: x-coordinate of */
    double* y;      /* y[i]: x-coordinate of */
    ll* Array;

    bool inputcsv;
    int Len;
    int Center;
    ll Magnification;
    ll INF;
    vector<vector<vector<double>>> Image;
};

#endif
