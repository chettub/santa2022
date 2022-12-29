#ifndef __INDI__
#define __INDI__

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using ll = long long;

class TIndi {
public:
    TIndi();
    ~TIndi();
    void define(ll N);
    TIndi& operator=(const TIndi& src);          /* Copy */
    bool operator==(const TIndi& indi2);         /* Return true if two tours are the same, false otherwise */
    ll next(const ll cur, const ll bef) const;   /* next vertex*/
    bool has_edge(const ll u, const ll v) const; /* check is has edge u-v */

    ll fN;               /* Number of cities */
    ll** fLink;          /* fLink[i][]: two vertices adjacent to i */
    ll fEvaluationValue; /* Tour length of */
};

#endif
