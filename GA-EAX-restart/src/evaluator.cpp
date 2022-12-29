#include <bits/stdc++.h>
#include <iostream>

#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

using namespace std;
using ll = long long;

int encode(int x, int y) {
    return x * 257 + y;
}
pair<int, int> decode(int i) {
    return make_pair(i / 257, i % 257);
}
int encodesmall(int dx, int dy) {
    dx += 8;
    dy += 8;
    return dx * 17 + dy;
}
pair<int, int> decodesmall(int i) {
    return make_pair((i / 17) - 8, (i % 17) - 8);
}
bool checkin(int x, int y) {
    return x == clamp(x, 0, 256) && y == clamp(y, 0, 256);
}

int quadrant(int idx) {
    auto [x, y] = decode(idx);
    return quadrant(x, y);
}
int quadrant(int x, int y) {
    if (x > 128 && y > 128)
        return 1;
    else if (x < 128 && y > 128)
        return 2;
    else if (x < 128 && y < 128)
        return 3;
    else if (x > 128 && y < 128)
        return 4;
    else
        return 0;
}


TEvaluator::TEvaluator() {
    fEdgeDis = NULL;
    fNearCity = NULL;
    Ncity = 0;
    fNearNumMax = 50;
    inputcsv = false;
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
        if (strcmp(word, "EDGE_WEIGHT_SECTION") == 0)
            break;
        if (strcmp(word, "x,y,r,g,b") == 0) {
            inputcsv = true;
            break;
        }
    }

    if (inputcsv) {
        Magnification = 1000000ll;
        INF = 100ll;
        Len = 257;
        Ncity = Len * Len;
        Center = 128;
        Image.assign(Len, vector<vector<double>>(Len, vector<double>(3, 0.)));
        int x, y;
        double r, g, b;
        for (int i = 0; i < Ncity; i++) {
            fscanf(fp, "%d,%d,%lf,%lf,%lf", &x, &y, &r, &g, &b);
            x += Center;
            y += Center;
            Image[x][y] = {r, g, b};
        }

        fEdgeDis = new ll*[Ncity];
        for (ll i = 0; i < Ncity; ++i) {
            fEdgeDis[i] = new ll[17 * 17];
            for (int j = 0; j < 17 * 17; j++) {
                fEdgeDis[i][j] = INF * Magnification;
            }
        }

        for (int x1 = 0; x1 < Len; x1++) {
            for (int y1 = 0; y1 < Len; y1++) {
                int idx1 = encode(x1, y1);
                for (int dx = -8; dx <= 8; dx++) {
                    for (int dy = -8; dy <= 8; dy++) {
                        if (abs(dx) + abs(dy) > 8)
                            continue;
                        int x2 = x1 + dx;
                        int y2 = y1 + dy;
                        if (!checkin(x2, y2))
                            continue;
                        int idx2 = encodesmall(dx, dy);
                        fEdgeDis[idx1][idx2] = (ll)(0.5 + Magnification * (sqrt(abs(dx) + abs(dy)) + 3 * (abs(Image[x1][y1][0] - Image[x2][y2][0]) + abs(Image[x1][y1][1] - Image[x2][y2][1]) + abs(Image[x1][y1][2] - Image[x2][y2][2]))));
                    }
                }
            }
        }

        // 初期移動制限
        // (0,-1) -> (0,0) -> (0,1)
        for (int dx = -8; dx <= 8; dx++) {
            for (int dy = -8; dy <= 8; dy++) {
                if (dx == 0 && abs(dy) == 1)
                    continue;
                {
                    int idx1 = encode(Center, Center);
                    int idx2 = encodesmall(dx, dy);
                    fEdgeDis[idx1][idx2] = INF * Magnification;
                }
                {
                    int idx1 = encode(Center + dx, Center + dy);
                    int idx2 = encodesmall(-dx, -dy);
                    fEdgeDis[idx1][idx2] = INF * Magnification;
                }
            }
        }

        fclose(fp);
        fNearCity = new ll*[Ncity];
        for (ll i = 0; i < Ncity; ++i)
            fNearCity[i] = new ll[fNearNumMax + 1];
        ll* checkedN = new ll[Ncity];
        ll ci, j1, j2, j3;
        ll cityNum = 0;
        ll minDis;
        for (ci = 0; ci < Ncity; ++ci) {
            for (j3 = 0; j3 < Ncity; ++j3)
                checkedN[j3] = 0;
            checkedN[ci] = 1;
            fNearCity[ci][0] = ci;
            auto [x1, y1] = decode(ci);
            for (j1 = 1; j1 <= fNearNumMax; ++j1) {
                minDis = 10000000000ll;
                // for (j2 = 0; j2 < Ncity; ++j2) {
                for (int dx = -8; dx <= 8; dx++) {
                    for (int dy = -8; dy <= 8; dy++) {
                        int x2 = x1 + dx;
                        int y2 = y1 + dy;
                        if (!checkin(x2, y2))
                            continue;

                        int j2 = encode(x2, y2);
                        if (funcEdgeDis(ci, j2) <= minDis && checkedN[j2] == 0) {
                            cityNum = j2;
                            minDis = funcEdgeDis(ci, j2);
                        }
                    }
                }
                // }
                fNearCity[ci][j1] = cityNum;
                checkedN[cityNum] = 1;
            }
        }

        return;

    } else {
        // std::cout << type << std::endl;
        // if (strcmp(word, "NODE_COORD_SECTION") != 0) {
        //     printf("Error in reading the instance\n");
        //     exit(0);
        // }
        x = new double[Ncity];
        y = new double[Ncity];

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
    }
    fclose(fp);
    fNearCity = new ll*[Ncity];
    for (ll i = 0; i < Ncity; ++i)
        fNearCity[i] = new ll[fNearNumMax + 1];
    ll* checkedN = new ll[Ncity];
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
                if (funcEdgeDis(ci, j2) <= minDis && checkedN[j2] == 0) {
                    cityNum = j2;
                    minDis = funcEdgeDis(ci, j2);
                }
            }
            fNearCity[ci][j1] = cityNum;
            checkedN[cityNum] = 1;
        }
    }
}


ll TEvaluator::funcEdgeDis(int i, int j) {
    if (inputcsv) {
        auto [x1, y1] = decode(i);
        auto [x2, y2] = decode(j);
        int dx = x2 - x1;
        int dy = y2 - y1;
        if (abs(dx) + abs(dy) > 8)
            return INF * Magnification;
        int idx1 = encode(x1, y1);
        int idx2 = encodesmall(dx, dy);
        return fEdgeDis[idx1][idx2];
    } else {
        return fEdgeDis[i][j];
    }
}

void TEvaluator::doIt(TIndi& indi) {
    ll d = 0;
    for (ll i = 0; i < Ncity; ++i)
        d += funcEdgeDis(i, indi.fLink[i][0]) + funcEdgeDis(i, indi.fLink[i][1]);
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
        distance += funcEdgeDis(array[i] - 1, array[i + 1] - 1);

    distance += funcEdgeDis(array[Ncity - 1] - 1, array[0] - 1);

    delete[] check;

    // NEW
    return true;
    // END

    if (distance != value)
        return false;
    return true;
}

ll TEvaluator::funcCostConstraintViolation(const TIndi& indi) const {
    if (GainConstraint == 0.0)
        return 0ll;
    int Nviolation = 0;
    // 原点回りの制約
    // (0,0)->(0,1)
    // (0,0)->(1,0)
    for (int curr : {indi.fLink[encode(Center, Center)][0], indi.fLink[encode(Center, Center)][1]}) {
        int pre = encode(Center, Center);
        int cntyplus = 0, cntyminus = 0;
        int cntxplus = 0, cntxminus = 0;
        do {
            auto [x1, y1] = decode(pre);
            auto [x2, y2] = decode(curr);
            cntxplus += x2 > x1;
            cntxminus += x2 < x1;
            if (x2 < Center) {
                Nviolation++;
                break;
            }
            if ((cntyplus + cntyminus) * 2 < cntxplus) {
                Nviolation++;
                break;
            }
            cntyplus += y2 > y1;
            cntyminus += y2 < y1;
            int next = indi.fLink[curr][0] + indi.fLink[curr][1] - pre;
            pre = curr;
            curr = next;
        } while (cntyplus < 64 && cntyminus < 64);
    }

    // (-127,-127)->(-127,127) 上で曲がる
    {
        bool violation = true;
        int x1 = 1;
        for (int y1 = 2; y1 < 255; y1 += 2) {
            int curr = encode(x1, y1);
            for (auto& next : {indi.fLink[curr][0], indi.fLink[curr][1]}) {
                auto [x2, y2] = decode(next);
                if (x2 != x1) {
                    violation = false;
                    break;
                }
            }
            if (!violation)
                break;
        }
        Nviolation += violation;
    }
    // (-127,127)->(127,127) 上で曲がる
    {
        bool violation = true;
        int y1 = 255;
        for (int x1 = 2; x1 < 255; x1 += 2) {
            int curr = encode(x1, y1);
            for (auto& next : {indi.fLink[curr][0], indi.fLink[curr][1]}) {
                auto [x2, y2] = decode(next);
                if (y2 != y1) {
                    violation = false;
                    break;
                }
            }
            if (!violation)
                break;
        }
        Nviolation += violation;
    }
    // (127,-127)->(127,127) 上で曲がる
    {
        bool violation = true;
        int x1 = 255;
        for (int y1 = 2; y1 < 255; y1 += 2) {
            int curr = encode(x1, y1);
            for (auto& next : {indi.fLink[curr][0], indi.fLink[curr][1]}) {
                auto [x2, y2] = decode(next);
                if (x2 != x1) {
                    violation = false;
                    break;
                }
            }
            if (!violation)
                break;
        }
        Nviolation += violation;
    }

    // slowing down speed of 4st -> 3rd -> 2nd -> 1st quadrant tour
    {
        for (int _ = 0; _ < 2; _++) {
            int xnow, xbef, ynow, ybef;
            int now, bef;
            if (_ == 0) {
                xnow = 128;
                xbef = 128 + 1;
                ynow = 128 - 4;
                ybef = 128 - 4;
            } else if (_ == 1) {
                xnow = 128;
                xbef = 128 + 1;
                ynow = 128 - 5;
                ybef = 128 - 5;
            } else
                assert(false);
            now = encode(xnow, ynow);
            bef = encode(xbef, ybef);

            if (!indi.has_edge(now, bef))
                continue;

            bool violation = false;

            int cntyplus = 0;
            vector<bool> isinquadrant(5, false);

            for (int __ = 0; __ < 2000; __++) {
                int nxt = indi.next(now, bef);
                bef = now;
                now = nxt;

                auto [xnow, ynow] = decode(now);
                auto [xbef, ybef] = decode(bef);

                int nowquad = quadrant(now);
                isinquadrant[nowquad] = true;

                if (isinquadrant[4])
                    break;
                if (isinquadrant[1]) {
                    if (isinquadrant[2] || isinquadrant[3]) {
                        violation = true;
                    }
                    break;
                }

                cntyplus += ynow > ybef;
                if (cntyplus >= 128)
                    break;
            }

            Nviolation += violation;
        }
    }

    ll cost = ll(GainConstraint * double(Magnification * Nviolation));
    return cost;
}