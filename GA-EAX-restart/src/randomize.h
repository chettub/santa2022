#ifndef __RAND__
#define __RAND__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using ll = long long;

extern void InitURandom(ll);
extern void InitURandom(void);

class TRandom {
public:
    TRandom();
    ~TRandom();
    ll Integer(ll minNumber, ll maxNumber);
    double Double(double minNumber, double maxNumber);
    double normalDistribution(double mu, double sigma);
    void permutation(ll* array, ll numOfelement, ll numOfSample);
    void shuffle(ll* array, ll numOfElement);
};

extern TRandom* tRand;

#endif
