#ifndef __SORT__
#define __SORT__

#include <algorithm>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

using ll = long long;

void InitSort();
void swap(ll& x, ll& y);
void selectionSort(ll* Arg, ll l, ll r);
ll partition(ll* Arg, ll l, ll r);
void quickSort(ll* Arg, ll l, ll r);

class TSort {
public:
    TSort();
    ~TSort();
    void index(double* Arg, ll numOfArg, ll* indexOrderd, ll numOfOrd);
    void index(ll* Arg, ll numOfArg, ll* indexOrderd, ll numOfOrd);
    void indexB(double* Arg, ll numOfArg, ll* indexOrderd, ll numOfOrd);
    void indexB(ll* Arg, ll numOfArg, ll* indexOrderd, ll numOfOrd);
    void sort(ll* Arg, ll numOfArg);
};

extern TSort* tSort;

#endif
