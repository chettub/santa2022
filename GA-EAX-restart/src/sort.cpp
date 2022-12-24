#ifndef __Sort__
#include "sort.h"
#endif

using ll = long long;

TSort* tSort = NULL;

void InitSort() {
    tSort = new TSort();
}

void swap(ll& x, ll& y) {
    ll s = x;
    x = y;
    y = s;
}

void selectionSort(ll* Arg, ll l, ll r) {
    ll id;
    for (ll i = l; i < r; ++i) {
        id = i;
        for (ll j = i + 1; j <= r; ++j)
            if (Arg[j] < Arg[id])
                id = j;
        swap(Arg[i], Arg[id]);
    }
}

ll partition(ll* Arg, ll l, ll r) {
    ll id = l + rand() % (r - l + 1);
    swap(Arg[l], Arg[id]);
    id = l;
    for (ll i = l + 1; i <= r; ++i)
        if (Arg[i] < Arg[l])
            swap(Arg[++id], Arg[i]);
    swap(Arg[l], Arg[id]);
    return id;
}

void quickSort(ll* Arg, ll l, ll r) {
    if (l < r) {
        if (r - l < 20) {
            selectionSort(Arg, l, r);
            return;
        }
        ll mid = partition(Arg, l, r);
        quickSort(Arg, l, mid - 1);
        quickSort(Arg, mid + 1, r);
    }
}

TSort::TSort() {}
TSort::~TSort() {}

void TSort::index(double* Arg, ll numOfArg, ll* indexOrderd, ll numOfOrd) {
    ll indexBest = 0;
    double valueBest;
    ll* checked = new ll[numOfArg];
    for (ll i = 0; i < numOfArg; ++i)
        checked[i] = 0;
    for (ll i = 0; i < numOfOrd; ++i) {
        valueBest = 99999999999.9;
        for (ll j = 0; j < numOfArg; ++j) {
            if ((Arg[j] < valueBest) && checked[j] == 0) {
                valueBest = Arg[j];
                indexBest = j;
            }
        }
        indexOrderd[i] = indexBest;
        checked[indexBest] = 1;
    }
    delete[] checked;
}

void TSort::indexB(double* Arg, ll numOfArg, ll* indexOrderd, ll numOfOrd) {
    ll indexBest = 0;
    double valueBest;
    ll* checked = new ll[numOfArg];
    for (ll i = 0; i < numOfArg; ++i)
        checked[i] = 0;
    for (ll i = 0; i < numOfOrd; ++i) {
        valueBest = -99999999999.9;
        for (ll j = 0; j < numOfArg; ++j) {
            if ((Arg[j] > valueBest) && checked[j] == 0) {
                valueBest = Arg[j];
                indexBest = j;
            }
        }
        indexOrderd[i] = indexBest;
        checked[indexBest] = 1;
    }
    delete[] checked;
}

void TSort::index(ll* Arg, ll numOfArg, ll* indexOrderd, ll numOfOrd) {
    ll indexBest = 0;
    ll valueBest;
    ll* checked = new ll[numOfArg];
    for (ll i = 0; i < numOfArg; ++i)
        checked[i] = 0;
    for (ll i = 0; i < numOfOrd; ++i) {
        valueBest = 99999999;
        for (ll j = 0; j < numOfArg; ++j) {
            if ((Arg[j] < valueBest) && checked[j] == 0) {
                valueBest = Arg[j];
                indexBest = j;
            }
        }
        indexOrderd[i] = indexBest;
        checked[indexBest] = 1;
    }
    delete[] checked;
}

void TSort::indexB(ll* Arg, ll numOfArg, ll* indexOrderd, ll numOfOrd) {
    ll indexBest = 0;
    ll valueBest;
    ll* checked = new ll[numOfArg];
    for (ll i = 0; i < numOfArg; ++i)
        checked[i] = 0;
    for (ll i = 0; i < numOfOrd; ++i) {
        valueBest = -999999999;
        for (ll j = 0; j < numOfArg; ++j) {
            if ((Arg[j] > valueBest) && checked[j] == 0) {
                valueBest = Arg[j];
                indexBest = j;
            }
        }
        indexOrderd[i] = indexBest;
        checked[indexBest] = 1;
    }
    delete[] checked;
}

void TSort::sort(ll* Arg, ll numOfArg) {
    // selectionSort(Arg, 0, numOfArg-1);
    quickSort(Arg, 0, numOfArg - 1);
}
