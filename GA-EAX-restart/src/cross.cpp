#ifndef __Cross__
#include "cross.h"
#endif

using ll = long long;

TCross::TCross(ll N) {
    fMaxNumOfABcycle = 4000; /* Set an appropriate value (2000 is usually enough) */
    fN = N;
    tBestTmp.define(fN);
    nearData = new ll*[fN];
    for (ll j = 0; j < fN; ++j)
        nearData[j] = new ll[5];

    fABcycle = new int*[fMaxNumOfABcycle];
    for (ll j = 0; j < fMaxNumOfABcycle; ++j)
        fABcycle[j] = new int[2 * fN + 4];

    koritsu = new ll[fN];
    bunki = new ll[fN];
    koriInv = new ll[fN];
    bunInv = new ll[fN];
    checkKoritsu = new ll[fN];
    fRoute = new ll[2 * fN + 1];
    fPermu = new ll[fMaxNumOfABcycle];

    fC = new ll[2 * fN + 4];
    fJun = new ll[fN + 1];
    fOrd1 = new ll[fN];
    fOrd2 = new ll[fN];

    // Speed Up Start
    fOrder = new ll[fN];
    fInv = new ll[fN];
    fSegment = new ll*[fN];
    for (ll j = 0; j < fN; ++j)
        fSegment[j] = new ll[2];

    fSegUnit = new ll[fN];
    fSegPosiList = new ll[fN];
    LinkAPosi = new ll[fN];
    LinkBPosi = new ll*[fN];
    for (ll j = 0; j < fN; ++j)
        LinkBPosi[j] = new ll[2];

    fPosiSeg = new ll[fN];
    fNumOfElementInUnit = new ll[fN];
    fCenterUnit = new ll[fN];
    for (ll j = 0; j < fN; ++j)
        fCenterUnit[j] = 0;

    fListOfCenterUnit = new ll[fN + 2];
    fSegForCenter = new ll[fN];
    fGainAB = new ll[fN];
    fModiEdge = new ll*[fN];
    for (ll j = 0; j < fN; ++j)
        fModiEdge[j] = new ll[4];

    fBestModiEdge = new ll*[fN];
    for (ll j = 0; j < fN; ++j)
        fBestModiEdge[j] = new ll[4];

    fAppliedCylce = new ll[fN];
    fBestAppliedCylce = new ll[fN];
    // Speed Up End

    // Block2
    fNumOfElementINAB = new ll[fMaxNumOfABcycle];
    fInEffectNode = new ll*[fN];
    for (ll i = 0; i < fN; ++i)
        fInEffectNode[i] = new ll[2];

    fWeightRR = new ll*[fMaxNumOfABcycle];
    for (ll i = 0; i < fMaxNumOfABcycle; ++i)
        fWeightRR[i] = new ll[fMaxNumOfABcycle];

    fWeightSR = new ll[fMaxNumOfABcycle];
    fWeightC = new ll[fMaxNumOfABcycle];
    fUsedAB = new ll[fN];
    fMovedAB = new ll[fN];
    fABcycleInEset = new ll[fMaxNumOfABcycle];
}

TCross::~TCross() {
    delete[] koritsu;
    delete[] bunki;
    delete[] koriInv;
    delete[] bunInv;
    delete[] checkKoritsu;
    delete[] fRoute;
    delete[] fPermu;
    for (ll j = 0; j < fN; ++j)
        delete[] nearData[j];

    delete[] nearData;
    for (ll j = 0; j < fMaxNumOfABcycle; ++j)
        delete[] fABcycle[j];

    delete[] fABcycle;
    delete[] fC;
    delete[] fJun;
    delete[] fOrd1;
    delete[] fOrd2;

    // Speed Up Start
    delete[] fOrder;
    delete[] fInv;

    for (ll j = 0; j < fN; ++j)
        delete[] fSegment[j];

    delete[] fSegment;
    delete[] fSegUnit;
    delete[] fSegPosiList;
    delete[] LinkAPosi;
    for (ll j = 0; j < fN; ++j)
        delete[] LinkBPosi[j];

    delete[] LinkBPosi;
    delete[] fPosiSeg;
    delete[] fNumOfElementInUnit;
    delete[] fCenterUnit;
    delete[] fListOfCenterUnit;
    delete[] fSegForCenter;
    delete[] fGainAB;

    for (ll j = 0; j < fN; ++j)
        delete[] fModiEdge[j];
    delete[] fModiEdge;
    for (ll j = 0; j < fN; ++j)
        delete[] fBestModiEdge[j];
    delete[] fBestModiEdge;

    delete[] fAppliedCylce;
    delete[] fBestAppliedCylce;
    // Speed Up End

    // Block2
    delete[] fNumOfElementINAB;
    for (ll j = 0; j < fN; ++j)
        delete[] fInEffectNode[j];

    delete[] fInEffectNode;
    for (ll i = 0; i < fMaxNumOfABcycle; ++i)
        delete[] fWeightRR[i];

    delete[] fWeightSR;
    delete[] fWeightC;
    delete[] fUsedAB;
    delete[] fMovedAB;
    delete[] fABcycleInEset;
}

void TCross::setParents(const TIndi& tPa1, const TIndi& tPa2, ll flagC[10], ll numOfKids) {
    this->setABcycle(tPa1, tPa2, flagC, numOfKids);
    fDisAB = 0;
    ll curr, next, st, pre;
    st = 0;
    curr = -1;
    next = st;
    for (ll i = 0; i < fN; ++i) {
        pre = curr;
        curr = next;
        if (tPa1.fLink[curr][0] != pre)
            next = tPa1.fLink[curr][0];
        else
            next = tPa1.fLink[curr][1];

        if (tPa2.fLink[curr][0] != next && tPa2.fLink[curr][1] != next)
            ++fDisAB;
        fOrder[i] = curr;
        fInv[curr] = i;
    }
    if (flagC[1] == 2) /* Block2 */
    {
        fTmax = 10;
        fMaxStag = 20;  // 1:Greedy LS, 20:Tabu Search
        this->setWeight(tPa1, tPa2);
    }
}

void TCross::doIt(TIndi& tKid, TIndi& tPa2, ll numOfKids, ll flagP, ll flagC[10], ll** fEdgeFreq) {
    ll Num;
    ll jnum, centerAB;
    ll gain;
    ll BestGain;
    double pollMax, poll;
    double DLoss;

    fEvalType = flagC[0];  // 1:Greedy, 2:---, 3:Distance, 4:Entropy
    fEsetType = flagC[1];  // 1:Single-AB, 2:Block2

    if (numOfKids <= fNumOfABcycle)
        Num = numOfKids;
    else
        Num = fNumOfABcycle;

    if (fEsetType == 1) /* Single-AB */
    {
        tRand->permutation(fPermu, fNumOfABcycle, fNumOfABcycle);
    }

    else if (fEsetType == 2) {  // Block2
        for (ll k = 0; k < fNumOfABcycle; ++k)
            fNumOfElementINAB[k] = fABcycle[k][0];
        tSort->indexB(fNumOfElementINAB, fNumOfABcycle, fPermu, fNumOfABcycle);
    }
    fNumOfGeneratedCh = 0;
    pollMax = 0.0;
    BestGain = 0;
    fFlagImp = 0;
    for (ll j = 0; j < Num; ++j) {
        fNumOfABcycleInEset = 0;
        if (fEsetType == 1) /* Single-AB */
        {
            jnum = fPermu[j];
            fABcycleInEset[fNumOfABcycleInEset++] = jnum;
        } else if (fEsetType == 2) /* Block2 */
        {
            jnum = fPermu[j];
            centerAB = jnum;
            for (ll s = 0; s < fNumOfABcycle; ++s) {
                if (s == centerAB)
                    fABcycleInEset[fNumOfABcycleInEset++] = s;
                else {
                    if (fWeightRR[centerAB][s] > 0 && fABcycle[s][0] < fABcycle[centerAB][0])
                        if (rand() % 2 == 0)
                            fABcycleInEset[fNumOfABcycleInEset++] = s;
                }
            }
            this->searchEset(centerAB);
        }
        fNumOfSPL = 0;
        gain = 0;
        fNumOfAppliedCycle = 0;
        fNumOfModiEdge = 0;

        fNumOfAppliedCycle = fNumOfABcycleInEset;
        for (ll k = 0; k < fNumOfAppliedCycle; ++k) {
            fAppliedCylce[k] = fABcycleInEset[k];
            jnum = fAppliedCylce[k];
            this->changeSol(tKid, jnum, flagP);
            gain += fGainAB[jnum];
        }

        this->makeUnit();
        this->makeCompleteSol(tKid);
        gain += fGainModi;

        ++fNumOfGeneratedCh;

        if (fEvalType == 1) /* Greedy */
            DLoss = 1.0;
        else if (fEvalType == 3) /* Distance preservation */
            DLoss = this->calAdpLoss(fEdgeFreq);
        else if (fEvalType == 4) /* Entropy preservation */
            DLoss = this->calEntLoss(fEdgeFreq);

        if (DLoss <= 0.0)
            DLoss = 0.00000001;

        poll = (double)gain / DLoss;
        tKid.fEvaluationValue = tKid.fEvaluationValue - gain;

        if (pollMax < poll && (2 * fBestNumE < fDisAB || tKid.fEvaluationValue != tPa2.fEvaluationValue)) {
            pollMax = poll;
            BestGain = gain;
            fFlagImp = 1;

            fNumOfBestAppliedCycle = fNumOfAppliedCycle;
            for (ll s = 0; s < fNumOfBestAppliedCycle; ++s)
                fBestAppliedCylce[s] = fAppliedCylce[s];

            fNumOfBestModiEdge = fNumOfModiEdge;
            for (ll s = 0; s < fNumOfBestModiEdge; ++s) {
                fBestModiEdge[s][0] = fModiEdge[s][0];
                fBestModiEdge[s][1] = fModiEdge[s][1];
                fBestModiEdge[s][2] = fModiEdge[s][2];
                fBestModiEdge[s][3] = fModiEdge[s][3];
            }
        }
        this->backToPa1(tKid);
        tKid.fEvaluationValue = tKid.fEvaluationValue + gain;
    }
    if (fFlagImp == 1) {
        this->goToBest(tKid);
        tKid.fEvaluationValue = tKid.fEvaluationValue - BestGain;
        this->incrementEdgeFreq(fEdgeFreq);
    }
}

void TCross::setABcycle(const TIndi& tPa1, const TIndi& tPa2, ll flagC[10], ll numOfKids) {
    bunkiMany = 0;
    koritsuMany = 0;
    for (ll j = 0; j < fN; ++j) {
        nearData[j][1] = tPa1.fLink[j][0];
        nearData[j][3] = tPa1.fLink[j][1];
        nearData[j][0] = 2;

        koritsu[koritsuMany] = j;
        koritsuMany++;

        nearData[j][2] = tPa2.fLink[j][0];
        nearData[j][4] = tPa2.fLink[j][1];
    }
    for (ll j = 0; j < fN; ++j) {
        checkKoritsu[j] = -1;
        koriInv[koritsu[j]] = j;
    }
    fNumOfABcycle = 0;
    flagSt = 1;
    while (koritsuMany != 0) {
        if (flagSt == 1) {
            fPosiCurr = 0;
            r = rand() % koritsuMany;
            st = koritsu[r];
            checkKoritsu[st] = fPosiCurr;
            fRoute[fPosiCurr] = st;
            ci = st;
            prType = 2;
        } else if (flagSt == 0)
            ci = fRoute[fPosiCurr];

        flagCycle = 0;
        while (flagCycle == 0) {
            fPosiCurr++;
            pr = ci;
            switch (prType) {
            case 1:
                ci = nearData[pr][fPosiCurr % 2 + 1];
                break;
            case 2:
                r = rand() % 2;
                ci = nearData[pr][fPosiCurr % 2 + 1 + 2 * r];
                if (r == 0)
                    this->swap(nearData[pr][fPosiCurr % 2 + 1], nearData[pr][fPosiCurr % 2 + 3]);
                break;
            case 3:
                ci = nearData[pr][fPosiCurr % 2 + 3];
            }
            fRoute[fPosiCurr] = ci;
            if (nearData[ci][0] == 2) {
                if (ci == st) {
                    if (checkKoritsu[st] == 0) {
                        if ((fPosiCurr - checkKoritsu[st]) % 2 == 0) {
                            if (nearData[st][fPosiCurr % 2 + 1] == pr)
                                this->swap(nearData[ci][fPosiCurr % 2 + 1], nearData[ci][fPosiCurr % 2 + 3]);

                            stAppear = 1;
                            this->formABcycle();
                            if (flagC[1] == 1 && fNumOfABcycle == numOfKids)
                                goto RETURN;
                            if (fNumOfABcycle == fMaxNumOfABcycle)
                                goto RETURN;

                            flagSt = 0;
                            flagCycle = 1;
                            prType = 1;
                        } else {
                            this->swap(nearData[ci][fPosiCurr % 2 + 1], nearData[ci][fPosiCurr % 2 + 3]);
                            prType = 2;
                        }
                        checkKoritsu[st] = fPosiCurr;
                    } else {
                        stAppear = 2;
                        this->formABcycle();
                        if (flagC[1] == 1 && fNumOfABcycle == numOfKids)
                            goto RETURN;
                        if (fNumOfABcycle == fMaxNumOfABcycle)
                            goto RETURN;

                        flagSt = 1;
                        flagCycle = 1;
                    }
                } else if (checkKoritsu[ci] == -1) {
                    checkKoritsu[ci] = fPosiCurr;
                    if (nearData[ci][fPosiCurr % 2 + 1] == pr)
                        this->swap(nearData[ci][fPosiCurr % 2 + 1], nearData[ci][fPosiCurr % 2 + 3]);
                    prType = 2;
                } else if (checkKoritsu[ci] > 0) {
                    this->swap(nearData[ci][fPosiCurr % 2 + 1], nearData[ci][fPosiCurr % 2 + 3]);
                    if ((fPosiCurr - checkKoritsu[ci]) % 2 == 0) {
                        stAppear = 1;
                        this->formABcycle();
                        if (flagC[1] == 1 && fNumOfABcycle == numOfKids)
                            goto RETURN;
                        if (fNumOfABcycle == fMaxNumOfABcycle)
                            goto RETURN;

                        flagSt = 0;
                        flagCycle = 1;
                        prType = 1;
                    } else {
                        this->swap(nearData[ci][(fPosiCurr + 1) % 2 + 1], nearData[ci][(fPosiCurr + 1) % 2 + 3]);
                        prType = 3;
                    }
                }
            } else if (nearData[ci][0] == 1) {
                if (ci == st) {
                    stAppear = 1;
                    this->formABcycle();
                    if (flagC[1] == 1 && fNumOfABcycle == numOfKids)
                        goto RETURN;
                    if (fNumOfABcycle == fMaxNumOfABcycle)
                        goto RETURN;
                    flagSt = 1;
                    flagCycle = 1;
                } else
                    prType = 1;
            }
        }
    }
    while (bunkiMany != 0) {
        fPosiCurr = 0;
        r = rand() % bunkiMany;
        st = bunki[r];
        fRoute[fPosiCurr] = st;
        ci = st;

        flagCycle = 0;
        while (flagCycle == 0) {
            pr = ci;
            fPosiCurr++;
            ci = nearData[pr][fPosiCurr % 2 + 1];
            fRoute[fPosiCurr] = ci;
            if (ci == st) {
                stAppear = 1;
                this->formABcycle();
                if (flagC[1] == 1 && fNumOfABcycle == numOfKids)
                    goto RETURN;
                if (fNumOfABcycle == fMaxNumOfABcycle)
                    goto RETURN;

                flagCycle = 1;
            }
        }
    }
RETURN:
    if (fNumOfABcycle == fMaxNumOfABcycle) {
        printf("fMaxNumOfABcycle(%lld) must be increased\n", fMaxNumOfABcycle);
        exit(1);
    }
}

void TCross::formABcycle() {
    ll j;
    ll st_count;
    ll edge_type;
    ll st, ci, stock;
    ll cem;
    ll diff;

    if (fPosiCurr % 2 == 0)
        edge_type = 1;
    else
        edge_type = 2;
    st = fRoute[fPosiCurr];
    cem = 0;
    fC[cem] = st;

    st_count = 0;
    while (1) {
        cem++;
        fPosiCurr--;
        ci = fRoute[fPosiCurr];
        if (nearData[ci][0] == 2) {
            koritsu[koriInv[ci]] = koritsu[koritsuMany - 1];
            koriInv[koritsu[koritsuMany - 1]] = koriInv[ci];
            koritsuMany--;
            bunki[bunkiMany] = ci;
            bunInv[ci] = bunkiMany;
            bunkiMany++;
        } else if (nearData[ci][0] == 1) {
            bunki[bunInv[ci]] = bunki[bunkiMany - 1];
            bunInv[bunki[bunkiMany - 1]] = bunInv[ci];
            bunkiMany--;
        }

        nearData[ci][0]--;
        if (ci == st)
            st_count++;
        if (st_count == stAppear)
            break;
        fC[cem] = ci;
    }

    if (cem == 2)
        return;

    fABcycle[fNumOfABcycle][0] = cem;

    if (edge_type == 2) {
        stock = fC[0];
        for (ll j = 0; j < cem - 1; j++)
            fC[j] = fC[j + 1];
        fC[cem - 1] = stock;
    }

    for (ll j = 0; j < cem; j++)
        fABcycle[fNumOfABcycle][j + 2] = fC[j];

    fABcycle[fNumOfABcycle][1] = fC[cem - 1];
    fABcycle[fNumOfABcycle][cem + 2] = fC[0];
    fABcycle[fNumOfABcycle][cem + 3] = fC[1];

    fC[cem] = fC[0];
    fC[cem + 1] = fC[1];
    diff = 0;
    for (j = 0; j < cem / 2; ++j)
        diff = diff + eval->fEdgeDis[fC[2 * j]][fC[1 + 2 * j]] - eval->fEdgeDis[fC[1 + 2 * j]][fC[2 + 2 * j]];

    fGainAB[fNumOfABcycle] = diff;
    ++fNumOfABcycle;
}

void TCross::swap(ll& x, ll& y) {
    ll s = x;
    x = y;
    y = s;
}

void TCross::changeSol(TIndi& tKid, ll ABnum, ll type) {
    ll j;
    ll cem, r1, r2, b1, b2;
    ll po_r1, po_r2, po_b1, po_b2;

    cem = fABcycle[ABnum][0];
    fC[0] = fABcycle[ABnum][0];

    if (type == 2)
        for (j = 0; j < cem + 3; j++)
            fC[cem + 3 - j] = fABcycle[ABnum][j + 1];
    else
        for (j = 1; j <= cem + 3; j++)
            fC[j] = fABcycle[ABnum][j];

    for (j = 0; j < cem / 2; j++) {
        r1 = fC[2 + 2 * j];
        r2 = fC[3 + 2 * j];
        b1 = fC[1 + 2 * j];
        b2 = fC[4 + 2 * j];

        if (tKid.fLink[r1][0] == r2)
            tKid.fLink[r1][0] = b1;
        else
            tKid.fLink[r1][1] = b1;
        if (tKid.fLink[r2][0] == r1)
            tKid.fLink[r2][0] = b2;
        else
            tKid.fLink[r2][1] = b2;

        po_r1 = fInv[r1];
        po_r2 = fInv[r2];
        po_b1 = fInv[b1];
        po_b2 = fInv[b2];

        if (po_r1 == 0 && po_r2 == fN - 1)
            fSegPosiList[fNumOfSPL++] = po_r1;
        else if (po_r1 == fN - 1 && po_r2 == 0)
            fSegPosiList[fNumOfSPL++] = po_r2;
        else if (po_r1 < po_r2)
            fSegPosiList[fNumOfSPL++] = po_r2;
        else if (po_r2 < po_r1)
            fSegPosiList[fNumOfSPL++] = po_r1;

        LinkBPosi[po_r1][1] = LinkBPosi[po_r1][0];
        LinkBPosi[po_r2][1] = LinkBPosi[po_r2][0];
        LinkBPosi[po_r1][0] = po_b1;
        LinkBPosi[po_r2][0] = po_b2;
    }
}

void TCross::makeCompleteSol(TIndi& tKid) {
    ll j, j1, j2;
    ll st, pre, curr, next, a, b, c, d, aa, bb, a1, b1;
    ll min_unit_city;
    ll center_un, select_un;
    ll diff, max_diff;
    ll near_num, nearMax;

    fGainModi = 0;
    while (fNumOfUnit != 1) {
        min_unit_city = fN + 12345;
        for (ll u = 0; u < fNumOfUnit; ++u)
            if (fNumOfElementInUnit[u] < min_unit_city) {
                center_un = u;
                min_unit_city = fNumOfElementInUnit[u];
            }


        st = -1;
        fNumOfSegForCenter = 0;
        for (ll s = 0; s < fNumOfSeg; ++s)
            if (fSegUnit[s] == center_un) {
                ll posi = fSegment[s][0];
                st = fOrder[posi];
                fSegForCenter[fNumOfSegForCenter++] = s;
            }
        curr = -1;
        next = st;
        fNumOfElementInCU = 0;
        while (1) {
            pre = curr;
            curr = next;
            fCenterUnit[curr] = 1;
            fListOfCenterUnit[fNumOfElementInCU] = curr;
            ++fNumOfElementInCU;
            if (tKid.fLink[curr][0] != pre)
                next = tKid.fLink[curr][0];
            else
                next = tKid.fLink[curr][1];
            if (next == st)
                break;
        }
        fListOfCenterUnit[fNumOfElementInCU] = fListOfCenterUnit[0];
        fListOfCenterUnit[fNumOfElementInCU + 1] = fListOfCenterUnit[1];

        max_diff = -999999999;
        a1 = -1;
        b1 = -1;
        nearMax = 10; /* N_near (see Step 5.3 in Section 2.2 of the Online Supplement) */
        /* nearMax must be smaller than or equal to eva->fNearNumMax (kopt.cpp ) */

    RESTART:
        for (ll s = 1; s <= fNumOfElementInCU; ++s) {
            a = fListOfCenterUnit[s];

            for (near_num = 1; near_num <= nearMax; ++near_num) {
                c = eval->fNearCity[a][near_num];
                if (fCenterUnit[c] == 0) {
                    for (j1 = 0; j1 < 2; ++j1) {
                        b = fListOfCenterUnit[s - 1 + 2 * j1];
                        for (j2 = 0; j2 < 2; ++j2) {
                            d = tKid.fLink[c][j2];
                            diff = eval->fEdgeDis[a][b] + eval->fEdgeDis[c][d] - eval->fEdgeDis[a][c] - eval->fEdgeDis[b][d];
                            if (diff > max_diff) {
                                aa = a;
                                bb = b;
                                a1 = c;
                                b1 = d;
                                max_diff = diff;
                            }
                            diff = eval->fEdgeDis[a][b] + eval->fEdgeDis[d][c] - eval->fEdgeDis[a][d] - eval->fEdgeDis[b][c];
                            if (diff > max_diff) {
                                aa = a;
                                bb = b;
                                a1 = d;
                                b1 = c;
                                max_diff = diff;
                            }
                        }
                    }
                }
            }
        }

        if (a1 == -1 && nearMax == 10) /* This value must also be changed if nearMax is chenged above */
        {
            nearMax = 50;
            goto RESTART;
        } else if (a1 == -1 && nearMax == 50) {
            ll r = rand() % (fNumOfElementInCU - 1);
            a = fListOfCenterUnit[r];
            b = fListOfCenterUnit[r + 1];
            for (j = 0; j < fN; ++j) {
                if (fCenterUnit[j] == 0) {
                    aa = a;
                    bb = b;
                    a1 = j;
                    b1 = tKid.fLink[j][0];
                    break;
                }
            }
            max_diff = eval->fEdgeDis[aa][bb] + eval->fEdgeDis[a1][b1] - eval->fEdgeDis[a][a1] - eval->fEdgeDis[b][b1];
        }

        if (tKid.fLink[aa][0] == bb)
            tKid.fLink[aa][0] = a1;
        else
            tKid.fLink[aa][1] = a1;
        if (tKid.fLink[bb][0] == aa)
            tKid.fLink[bb][0] = b1;
        else
            tKid.fLink[bb][1] = b1;
        if (tKid.fLink[a1][0] == b1)
            tKid.fLink[a1][0] = aa;
        else
            tKid.fLink[a1][1] = aa;
        if (tKid.fLink[b1][0] == a1)
            tKid.fLink[b1][0] = bb;
        else
            tKid.fLink[b1][1] = bb;

        fModiEdge[fNumOfModiEdge][0] = aa;
        fModiEdge[fNumOfModiEdge][1] = bb;
        fModiEdge[fNumOfModiEdge][2] = a1;
        fModiEdge[fNumOfModiEdge][3] = b1;
        ++fNumOfModiEdge;

        fGainModi += max_diff;

        ll posi_a1 = fInv[a1];
        select_un = -1;
        for (ll s = 0; s < fNumOfSeg; ++s)
            if (fSegment[s][0] <= posi_a1 && posi_a1 <= fSegment[s][1]) {
                select_un = fSegUnit[s];
                break;
            }


        for (ll s = 0; s < fNumOfSeg; ++s)
            if (fSegUnit[s] == select_un)
                fSegUnit[s] = center_un;

        fNumOfElementInUnit[center_un] += fNumOfElementInUnit[select_un];

        for (ll s = 0; s < fNumOfSeg; ++s)
            if (fSegUnit[s] == fNumOfUnit - 1)
                fSegUnit[s] = select_un;

        fNumOfElementInUnit[select_un] = fNumOfElementInUnit[fNumOfUnit - 1];
        --fNumOfUnit;

        for (ll s = 0; s < fNumOfElementInCU; ++s) {
            c = fListOfCenterUnit[s];
            fCenterUnit[c] = 0;
        }
    }
}

void TCross::makeUnit() {
    ll flag = 1;
    for (ll s = 0; s < fNumOfSPL; ++s) {
        if (fSegPosiList[s] == 0) {
            flag = 0;
            break;
        }
    }
    if (flag == 1) {
        fSegPosiList[fNumOfSPL++] = 0;
        LinkBPosi[fN - 1][1] = LinkBPosi[fN - 1][0];
        LinkBPosi[0][1] = LinkBPosi[0][0];
        LinkBPosi[fN - 1][0] = 0;
        LinkBPosi[0][0] = fN - 1;
    }

    tSort->sort(fSegPosiList, fNumOfSPL);
    fNumOfSeg = fNumOfSPL;
    for (ll s = 0; s < fNumOfSeg - 1; ++s) {
        fSegment[s][0] = fSegPosiList[s];
        fSegment[s][1] = fSegPosiList[s + 1] - 1;
    }

    fSegment[fNumOfSeg - 1][0] = fSegPosiList[fNumOfSeg - 1];
    fSegment[fNumOfSeg - 1][1] = fN - 1;

    for (ll s = 0; s < fNumOfSeg; ++s) {
        LinkAPosi[fSegment[s][0]] = fSegment[s][1];
        LinkAPosi[fSegment[s][1]] = fSegment[s][0];
        fPosiSeg[fSegment[s][0]] = s;
        fPosiSeg[fSegment[s][1]] = s;
    }

    for (ll s = 0; s < fNumOfSeg; ++s)
        fSegUnit[s] = -1;
    fNumOfUnit = 0;

    ll p_st, p1, p2, p_next, p_pre;
    ll segNum;
    while (1) {
        flag = 0;
        for (ll s = 0; s < fNumOfSeg; ++s) {
            if (fSegUnit[s] == -1) {
                p_st = fSegment[s][0];
                p_pre = -1;
                p1 = p_st;
                flag = 1;
                break;
            }
        }
        if (flag == 0)
            break;

        while (1) {
            segNum = fPosiSeg[p1];
            fSegUnit[segNum] = fNumOfUnit;

            p2 = LinkAPosi[p1];
            p_next = LinkBPosi[p2][0];
            if (p1 == p2)
                if (p_next == p_pre)
                    p_next = LinkBPosi[p2][1];

            if (p_next == p_st) {
                ++fNumOfUnit;
                break;
            }

            p_pre = p2;
            p1 = p_next;
        }
    }

    for (ll s = 0; s < fNumOfUnit; ++s)
        fNumOfElementInUnit[s] = 0;

    ll unitNum = -1;
    ll tmpNumOfSeg = -1;
    for (ll s = 0; s < fNumOfSeg; ++s) {
        if (fSegUnit[s] != unitNum) {
            ++tmpNumOfSeg;
            fSegment[tmpNumOfSeg][0] = fSegment[s][0];
            fSegment[tmpNumOfSeg][1] = fSegment[s][1];
            unitNum = fSegUnit[s];
            fSegUnit[tmpNumOfSeg] = unitNum;
            fNumOfElementInUnit[unitNum] += fSegment[s][1] - fSegment[s][0] + 1;
        } else {
            fSegment[tmpNumOfSeg][1] = fSegment[s][1];
            fNumOfElementInUnit[unitNum] += fSegment[s][1] - fSegment[s][0] + 1;
        }
    }
    fNumOfSeg = tmpNumOfSeg + 1;
}

void TCross::backToPa1(TIndi& tKid) {
    ll aa, bb, a1, b1;
    ll jnum;

    for (ll s = fNumOfModiEdge - 1; s >= 0; --s) {
        aa = fModiEdge[s][0];
        a1 = fModiEdge[s][1];
        bb = fModiEdge[s][2];
        b1 = fModiEdge[s][3];

        if (tKid.fLink[aa][0] == bb)
            tKid.fLink[aa][0] = a1;
        else
            tKid.fLink[aa][1] = a1;
        if (tKid.fLink[b1][0] == a1)
            tKid.fLink[b1][0] = bb;
        else
            tKid.fLink[b1][1] = bb;
        if (tKid.fLink[bb][0] == aa)
            tKid.fLink[bb][0] = b1;
        else
            tKid.fLink[bb][1] = b1;
        if (tKid.fLink[a1][0] == b1)
            tKid.fLink[a1][0] = aa;
        else
            tKid.fLink[a1][1] = aa;
    }

    for (ll s = 0; s < fNumOfAppliedCycle; ++s) {
        jnum = fAppliedCylce[s];
        this->changeSol(tKid, jnum, 2);
    }
}

void TCross::goToBest(TIndi& tKid) {
    ll aa, bb, a1, b1;
    ll jnum;

    for (ll s = 0; s < fNumOfBestAppliedCycle; ++s) {
        jnum = fBestAppliedCylce[s];
        this->changeSol(tKid, jnum, 1);
    }

    for (ll s = 0; s < fNumOfBestModiEdge; ++s) {
        aa = fBestModiEdge[s][0];
        bb = fBestModiEdge[s][1];
        a1 = fBestModiEdge[s][2];
        b1 = fBestModiEdge[s][3];

        if (tKid.fLink[aa][0] == bb)
            tKid.fLink[aa][0] = a1;
        else
            tKid.fLink[aa][1] = a1;
        if (tKid.fLink[bb][0] == aa)
            tKid.fLink[bb][0] = b1;
        else
            tKid.fLink[bb][1] = b1;
        if (tKid.fLink[a1][0] == b1)
            tKid.fLink[a1][0] = aa;
        else
            tKid.fLink[a1][1] = aa;
        if (tKid.fLink[b1][0] == a1)
            tKid.fLink[b1][0] = bb;
        else
            tKid.fLink[b1][1] = bb;
    }
}

void TCross::incrementEdgeFreq(ll** fEdgeFreq) {
    ll j, jnum, cem;
    ll r1, r2, b1, b2;
    ll aa, bb, a1;

    for (ll s = 0; s < fNumOfBestAppliedCycle; ++s) {
        jnum = fBestAppliedCylce[s];

        cem = fABcycle[jnum][0];
        fC[0] = fABcycle[jnum][0];

        for (j = 1; j <= cem + 3; ++j)
            fC[j] = fABcycle[jnum][j];

        for (j = 0; j < cem / 2; ++j) {
            r1 = fC[2 + 2 * j];
            r2 = fC[3 + 2 * j];
            b1 = fC[1 + 2 * j];
            b2 = fC[4 + 2 * j];

            // r1 - b1 add
            // r1 - r2 remove
            // r2 - r1 remove
            // r2 - b2 add

            ++fEdgeFreq[r1][b1];
            --fEdgeFreq[r1][r2];
            --fEdgeFreq[r2][r1];
            ++fEdgeFreq[r2][b2];
        }
    }
    for (ll s = 0; s < fNumOfBestModiEdge; ++s) {
        aa = fBestModiEdge[s][0];
        bb = fBestModiEdge[s][1];
        a1 = fBestModiEdge[s][2];
        b1 = fBestModiEdge[s][3];

        --fEdgeFreq[aa][bb];
        --fEdgeFreq[a1][b1];
        ++fEdgeFreq[aa][a1];
        ++fEdgeFreq[bb][b1];
        --fEdgeFreq[bb][aa];
        --fEdgeFreq[b1][a1];
        ++fEdgeFreq[a1][aa];
        ++fEdgeFreq[b1][bb];
    }
}

ll TCross::calAdpLoss(ll** fEdgeFreq) {
    ll j, jnum, cem;
    ll r1, r2, b1, b2;
    ll aa, bb, a1;
    double DLoss;

    DLoss = 0;
    for (ll s = 0; s < fNumOfAppliedCycle; ++s) {
        jnum = fAppliedCylce[s];

        cem = fABcycle[jnum][0];
        fC[0] = fABcycle[jnum][0];

        for (j = 1; j <= cem + 3; ++j)
            fC[j] = fABcycle[jnum][j];

        for (j = 0; j < cem / 2; ++j) {
            r1 = fC[2 + 2 * j];
            r2 = fC[3 + 2 * j];
            b1 = fC[1 + 2 * j];
            b2 = fC[4 + 2 * j];

            // r1 - b1 add
            // r1 - r2 remove
            // r2 - r1 remove
            // r2 - b2 add

            DLoss -= (fEdgeFreq[r1][r2] - 1);
            DLoss -= (fEdgeFreq[r2][r1] - 1);
            DLoss += fEdgeFreq[r2][b2];
            DLoss += fEdgeFreq[b2][r2];

            // Remove
            --fEdgeFreq[r1][r2];
            --fEdgeFreq[r2][r1];

            // Add
            ++fEdgeFreq[r2][b2];
            ++fEdgeFreq[b2][r2];
        }
    }

    for (ll s = 0; s < fNumOfModiEdge; ++s) {
        aa = fModiEdge[s][0];
        bb = fModiEdge[s][1];
        a1 = fModiEdge[s][2];
        b1 = fModiEdge[s][3];

        DLoss -= (fEdgeFreq[aa][bb] - 1);
        DLoss -= (fEdgeFreq[bb][aa] - 1);
        DLoss -= (fEdgeFreq[a1][b1] - 1);
        DLoss -= (fEdgeFreq[b1][a1] - 1);

        DLoss += fEdgeFreq[aa][a1];
        DLoss += fEdgeFreq[a1][aa];
        DLoss += fEdgeFreq[bb][b1];
        DLoss += fEdgeFreq[b1][bb];

        // Remove
        --fEdgeFreq[aa][bb];
        --fEdgeFreq[bb][aa];
        --fEdgeFreq[a1][b1];
        --fEdgeFreq[b1][a1];

        // Add
        ++fEdgeFreq[aa][a1];
        ++fEdgeFreq[a1][aa];
        ++fEdgeFreq[bb][b1];
        ++fEdgeFreq[b1][bb];
    }

    for (ll s = 0; s < fNumOfAppliedCycle; ++s) {
        jnum = fAppliedCylce[s];
        cem = fABcycle[jnum][0];
        fC[0] = fABcycle[jnum][0];
        for (j = 1; j <= cem + 3; ++j)
            fC[j] = fABcycle[jnum][j];

        for (j = 0; j < cem / 2; ++j) {
            r1 = fC[2 + 2 * j];
            r2 = fC[3 + 2 * j];
            b1 = fC[1 + 2 * j];
            b2 = fC[4 + 2 * j];

            ++fEdgeFreq[r1][r2];
            ++fEdgeFreq[r2][r1];
            --fEdgeFreq[r2][b2];
            --fEdgeFreq[b2][r2];
        }
    }

    // Modification
    for (ll s = 0; s < fNumOfModiEdge; ++s) {
        aa = fModiEdge[s][0];
        bb = fModiEdge[s][1];
        a1 = fModiEdge[s][2];
        b1 = fModiEdge[s][3];

        // Remove
        ++fEdgeFreq[aa][bb];
        ++fEdgeFreq[bb][aa];
        ++fEdgeFreq[a1][b1];
        ++fEdgeFreq[b1][a1];

        --fEdgeFreq[aa][a1];
        --fEdgeFreq[a1][aa];
        --fEdgeFreq[bb][b1];
        --fEdgeFreq[b1][bb];
    }
    return ll(DLoss / 2);
}

double TCross::calEntLoss(ll** fEdgeFreq) {
    ll j, jnum, cem;
    ll r1, r2, b1, b2;
    ll aa, bb, a1;
    double DLoss;
    double h1, h2;

    DLoss = 0;
    // AB-cycle
    for (ll s = 0; s < fNumOfAppliedCycle; ++s) {
        jnum = fAppliedCylce[s];
        cem = fABcycle[jnum][0];
        fC[0] = fABcycle[jnum][0];

        for (j = 1; j <= cem + 3; ++j)
            fC[j] = fABcycle[jnum][j];

        for (j = 0; j < cem / 2; ++j) {
            r1 = fC[2 + 2 * j];
            r2 = fC[3 + 2 * j];
            b1 = fC[1 + 2 * j];
            b2 = fC[4 + 2 * j];

            // r1 - b1 add
            // r1 - r2 remove
            // r2 - r1 remove
            // r2 - b2 add

            // Remove
            h1 = (double)(fEdgeFreq[r1][r2] - 1) / (double)Npop;
            h2 = (double)(fEdgeFreq[r1][r2]) / (double)Npop;
            if (fEdgeFreq[r1][r2] - 1 != 0)
                DLoss -= h1 * log(h1);
            DLoss += h2 * log(h2);
            --fEdgeFreq[r1][r2];
            --fEdgeFreq[r2][r1];

            // Add
            h1 = (double)(fEdgeFreq[r2][b2] + 1) / (double)Npop;
            h2 = (double)(fEdgeFreq[r2][b2]) / (double)Npop;
            DLoss -= h1 * log(h1);
            if (fEdgeFreq[r2][b2] != 0)
                DLoss += h2 * log(h2);
            ++fEdgeFreq[r2][b2];
            ++fEdgeFreq[b2][r2];
        }
    }

    // Modification
    for (ll s = 0; s < fNumOfModiEdge; ++s) {
        aa = fModiEdge[s][0];
        bb = fModiEdge[s][1];
        a1 = fModiEdge[s][2];
        b1 = fModiEdge[s][3];

        h1 = (double)(fEdgeFreq[aa][bb] - 1) / (double)Npop;
        h2 = (double)(fEdgeFreq[aa][bb]) / (double)Npop;
        if (fEdgeFreq[aa][bb] - 1 != 0)
            DLoss -= h1 * log(h1);
        DLoss += h2 * log(h2);
        --fEdgeFreq[aa][bb];
        --fEdgeFreq[bb][aa];

        h1 = (double)(fEdgeFreq[a1][b1] - 1) / (double)Npop;
        h2 = (double)(fEdgeFreq[a1][b1]) / (double)Npop;
        if (fEdgeFreq[a1][b1] - 1 != 0)
            DLoss -= h1 * log(h1);
        DLoss += h2 * log(h2);
        --fEdgeFreq[a1][b1];
        --fEdgeFreq[b1][a1];

        h1 = (double)(fEdgeFreq[aa][a1] + 1) / (double)Npop;
        h2 = (double)(fEdgeFreq[aa][a1]) / (double)Npop;
        DLoss -= h1 * log(h1);
        if (fEdgeFreq[aa][a1] != 0)
            DLoss += h2 * log(h2);
        ++fEdgeFreq[aa][a1];
        ++fEdgeFreq[a1][aa];

        h1 = (double)(fEdgeFreq[bb][b1] + 1) / (double)Npop;
        h2 = (double)(fEdgeFreq[bb][b1]) / (double)Npop;
        DLoss -= h1 * log(h1);
        if (fEdgeFreq[bb][b1] != 0)
            DLoss += h2 * log(h2);
        ++fEdgeFreq[bb][b1];
        ++fEdgeFreq[b1][bb];
    }
    DLoss = -DLoss;

    for (ll s = 0; s < fNumOfAppliedCycle; ++s) {
        jnum = fAppliedCylce[s];

        cem = fABcycle[jnum][0];
        fC[0] = fABcycle[jnum][0];

        for (j = 1; j <= cem + 3; ++j)
            fC[j] = fABcycle[jnum][j];

        for (j = 0; j < cem / 2; ++j) {
            r1 = fC[2 + 2 * j];
            r2 = fC[3 + 2 * j];
            b1 = fC[1 + 2 * j];
            b2 = fC[4 + 2 * j];

            ++fEdgeFreq[r1][r2];
            ++fEdgeFreq[r2][r1];
            --fEdgeFreq[r2][b2];
            --fEdgeFreq[b2][r2];
        }
    }
    for (ll s = 0; s < fNumOfModiEdge; ++s) {
        aa = fModiEdge[s][0];
        bb = fModiEdge[s][1];
        a1 = fModiEdge[s][2];
        b1 = fModiEdge[s][3];

        ++fEdgeFreq[aa][bb];
        ++fEdgeFreq[bb][aa];
        ++fEdgeFreq[a1][b1];
        ++fEdgeFreq[b1][a1];

        --fEdgeFreq[aa][a1];
        --fEdgeFreq[a1][aa];
        --fEdgeFreq[bb][b1];
        --fEdgeFreq[b1][bb];
    }
    return DLoss;
}

void TCross::setWeight(const TIndi& tPa1, const TIndi& tPa2) {
    ll cem;
    ll r1, r2, v1, v2, v_p;
    ll AB_num;

    for (ll i = 0; i < fN; ++i) {
        fInEffectNode[i][0] = -1;
        fInEffectNode[i][1] = -1;
    }

    // Step 1:
    for (ll s = 0; s < fNumOfABcycle; ++s) {
        cem = fABcycle[s][0];
        for (ll j = 0; j < cem / 2; ++j) {
            r1 = fABcycle[s][2 * j + 2];  // red edge
            r2 = fABcycle[s][2 * j + 3];

            if (fInEffectNode[r1][0] == -1)
                fInEffectNode[r1][0] = s;
            else if (fInEffectNode[r1][1] == -1)
                fInEffectNode[r1][1] = s;


            if (fInEffectNode[r2][0] == -1)
                fInEffectNode[r2][0] = s;
            else if (fInEffectNode[r2][1] == -1)
                fInEffectNode[r2][1] = s;
        }
    }

    // Step 2:
    for (ll i = 0; i < fN; ++i) {
        if (fInEffectNode[i][0] != -1 && fInEffectNode[i][1] == -1) {
            AB_num = fInEffectNode[i][0];
            v1 = i;

            if (tPa1.fLink[v1][0] != tPa2.fLink[v1][0] && tPa1.fLink[v1][0] != tPa2.fLink[v1][1])
                v_p = tPa1.fLink[v1][0];
            else if (tPa1.fLink[v1][1] != tPa2.fLink[v1][0] && tPa1.fLink[v1][1] != tPa2.fLink[v1][1])
                v_p = tPa1.fLink[v1][1];


            while (1) {
                fInEffectNode[v1][1] = AB_num;

                if (tPa1.fLink[v1][0] != v_p)
                    v2 = tPa1.fLink[v1][0];
                else if (tPa1.fLink[v1][1] != v_p)
                    v2 = tPa1.fLink[v1][1];


                if (fInEffectNode[v2][0] == -1)
                    fInEffectNode[v2][0] = AB_num;
                else if (fInEffectNode[v2][1] == -1)
                    fInEffectNode[v2][1] = AB_num;


                if (fInEffectNode[v2][1] != -1)
                    break;
                v_p = v1;
                v1 = v2;
            }
        }
    }

    // Step 3:

    for (ll s1 = 0; s1 < fNumOfABcycle; ++s1) {
        fWeightC[s1] = 0;
        for (ll s2 = 0; s2 < fNumOfABcycle; ++s2)
            fWeightRR[s1][s2] = 0;
    }

    for (ll i = 0; i < fN; ++i) {

        if (fInEffectNode[i][0] != -1 && fInEffectNode[i][1] != -1) {
            ++fWeightRR[fInEffectNode[i][0]][fInEffectNode[i][1]];
            ++fWeightRR[fInEffectNode[i][1]][fInEffectNode[i][0]];
        }
        if (fInEffectNode[i][0] != fInEffectNode[i][1]) {
            ++fWeightC[fInEffectNode[i][0]];
            ++fWeightC[fInEffectNode[i][1]];
        }
    }
    for (ll s1 = 0; s1 < fNumOfABcycle; ++s1)
        fWeightRR[s1][s1] = 0;
}

ll TCross::calCNaive() {
    ll count_C;
    ll tt;

    count_C = 0;

    for (ll i = 0; i < fN; ++i) {
        if (fInEffectNode[i][0] != -1 && fInEffectNode[i][1] != -1) {
            tt = 0;
            if (fUsedAB[fInEffectNode[i][0]] == 1)
                ++tt;
            if (fUsedAB[fInEffectNode[i][1]] == 1)
                ++tt;
            if (tt == 1)
                ++count_C;
        }
    }
    return count_C;
}

void TCross::searchEset(ll centerAB) {
    ll nIter, stagImp;
    ll delta_weight, min_delta_weight_nt;
    ll flag_AddDelete, flag_AddDelete_nt;
    ll selected_AB, selected_AB_nt;
    ll jnum;

    fNumC = 0;  // Number of C nodes in E-set
    fNumE = 0;  // Number of Edges in E-set

    fNumOfUsedAB = 0;
    for (ll s1 = 0; s1 < fNumOfABcycle; ++s1) {
        fUsedAB[s1] = 0;
        fWeightSR[s1] = 0;
        fMovedAB[s1] = 0;
    }

    for (ll s = 0; s < fNumOfABcycleInEset; ++s) {
        jnum = fABcycleInEset[s];
        this->addAB(jnum);
    }
    fBestNumC = fNumC;
    fBestNumE = fNumE;

    stagImp = 0;
    nIter = 0;
    while (1) {
        ++nIter;
        min_delta_weight_nt = 99999999;
        flag_AddDelete = 0;
        flag_AddDelete_nt = 0;
        for (ll s1 = 0; s1 < fNumOfABcycle; ++s1) {
            if (fUsedAB[s1] == 0 && fWeightSR[s1] > 0) {
                delta_weight = fWeightC[s1] - 2 * fWeightSR[s1];
                if (fNumC + delta_weight < fBestNumC) {
                    selected_AB = s1;
                    flag_AddDelete = 1;
                    fBestNumC = fNumC + delta_weight;
                }
                if (delta_weight < min_delta_weight_nt && nIter > fMovedAB[s1]) {
                    selected_AB_nt = s1;
                    flag_AddDelete_nt = 1;
                    min_delta_weight_nt = delta_weight;
                }
            } else if (fUsedAB[s1] == 1 && s1 != centerAB) {
                delta_weight = -fWeightC[s1] + 2 * fWeightSR[s1];
                if (fNumC + delta_weight < fBestNumC) {
                    selected_AB = s1;
                    flag_AddDelete = -1;
                    fBestNumC = fNumC + delta_weight;
                }
                if (delta_weight < min_delta_weight_nt && nIter > fMovedAB[s1]) {
                    selected_AB_nt = s1;
                    flag_AddDelete_nt = -1;
                    min_delta_weight_nt = delta_weight;
                }
            }
        }

        if (flag_AddDelete != 0) {
            if (flag_AddDelete == 1)
                this->addAB(selected_AB);
            else if (flag_AddDelete == -1)
                this->deleteAB(selected_AB);

            fMovedAB[selected_AB] = nIter + tRand->Integer(1, fTmax);

            fBestNumE = fNumE;

            fNumOfABcycleInEset = 0;
            for (ll s1 = 0; s1 < fNumOfABcycle; ++s1)
                if (fUsedAB[s1] == 1)
                    fABcycleInEset[fNumOfABcycleInEset++] = s1;

            stagImp = 0;
        } else if (flag_AddDelete_nt != 0) {
            if (flag_AddDelete_nt == 1)
                this->addAB(selected_AB_nt);
            else if (flag_AddDelete_nt == -1)

                this->deleteAB(selected_AB_nt);
            fMovedAB[selected_AB_nt] = nIter + tRand->Integer(1, fTmax);
        }
        if (flag_AddDelete == 0)
            ++stagImp;
        if (stagImp == fMaxStag)
            break;
    }
}

void TCross::addAB(ll num) {
    fNumC += fWeightC[num] - 2 * fWeightSR[num];
    fNumE += fABcycle[num][0] / 2;

    fUsedAB[num] = 1;
    ++fNumOfUsedAB;
    for (ll s1 = 0; s1 < fNumOfABcycle; ++s1)
        fWeightSR[s1] += fWeightRR[s1][num];
}

void TCross::deleteAB(ll num) {
    fNumC -= fWeightC[num] - 2 * fWeightSR[num];
    fNumE -= fABcycle[num][0] / 2;

    fUsedAB[num] = 0;
    --fNumOfUsedAB;
    for (ll s1 = 0; s1 < fNumOfABcycle; ++s1)
        fWeightSR[s1] -= fWeightRR[s1][num];
}
