#include <bits/stdc++.h>
using namespace std;
using lint = long long;

class XorShift {
    uint32_t x, y, z, w, t;

public:
    XorShift() {
        x = 133553533;
        y = 314867339;
        z = 664298413;
        w = 999999937;
        t = 0;
    }
    // [0, 2^32)
    inline uint32_t operator()(void) {
        t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        return w;
    }
    // [0, u)
    int operator()(int u) { return (lint(this->operator()()) * u) >> 32; }
    // [l, r)
    int operator()(int l, int r) { return l + this->operator()(r - l); }
    // [0.0, 1.0]
    double prob() { return (this->operator()() & 0xfffffff) * 3.7252903123397e-9; }
};

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

constexpr long double inf = 100.0;
constexpr int len = 257;
constexpr int n_city = len * len;
constexpr int center = 128;

long double average_cost = 0.0;
long double entropy = 0.0;
vector<map<int, int>> edge_count(n_city);
long double over_npop;
vector<vector<vector<long double>>> image(len, vector<vector<long double>>(len, vector<long double>(3, 0.0)));
vector<vector<long double>> fEdgeDis(n_city, vector<long double>(17 * 17, inf));
XorShift rnd;

vector<int> string_to_vector_int(string& str, const char delim = ' ') {
    vector<int> vec;

    replace(str.begin(), str.end(), delim, ' ');
    istringstream iss(str);

    string str_buf;
    while (iss >> str_buf) {
        if (str_buf == "")
            continue;
        vec.emplace_back(stoi(str_buf) - 1);
    }

    return vec;
}

inline long double log_fast(long double x) {
    long double xm1 = x - 1.0;
    return xm1 * (1.0 - xm1 * (0.5 - 0.333333 * xm1));
}

void addEdge(int i, int j) {
    int cnt = edge_count[i][j];
    long double r = 0.0;
    if (cnt != 0) {
        r = (long double)(cnt)*over_npop;
        entropy -= r * log_fast(r);
    }
    edge_count[i][j]++;
    cnt++;
    r = (long double)(cnt)*over_npop;
    entropy += r * log_fast(r);
}

void removeEdge(int i, int j) {
    int cnt = edge_count[i][j];
    long double r = 0.0;
    if (cnt != 0) {
        r = (long double)(cnt)*over_npop;
        entropy -= r * log_fast(r);
    }
    edge_count[i][j]--;
    assert(edge_count[i][j] >= 0);
    cnt--;
    if (cnt != 0) {
        r = (long double)(cnt)*over_npop;
        entropy += r * log_fast(r);
    }
}

void addEdgeAll(vector<int>& route) {
    for (int k = 0; k < n_city - 1; k++) {
        addEdge(route[k], route[k + 1]);
        addEdge(route[k + 1], route[k]);
    }
    addEdge(route[n_city - 1], route[0]);
    addEdge(route[0], route[n_city - 1]);
}

void removeEdgeAll(vector<int>& route) {
    for (int m = 0; m < n_city - 1; m++) {
        removeEdge(route[m], route[m + 1]);
        removeEdge(route[m + 1], route[m]);
    }
    removeEdge(route[n_city - 1], route[0]);
    removeEdge(route[0], route[n_city - 1]);
}

long double funcEdgeDis(int i, int j) {
    auto [x1, y1] = decode(i);
    auto [x2, y2] = decode(j);
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (abs(dx) + abs(dy) > 8)
        return inf;
    int idx1 = encode(x1, y1);
    int idx2 = encodesmall(dx, dy);
    return fEdgeDis[idx1][idx2];
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

int quadrant(int idx) {
    auto [x, y] = decode(idx);
    return quadrant(x, y);
}

double penalty(vector<int>& path) {
    int center = encode(128, 128);
    int corner1 = encode(1, 1);
    int corner2 = encode(255, 255);

    double penalty = 0;

    int cnt = 0;
    for (int i = 0; i < path.size(); i++) {
        if (path[i] == center) {
            cnt++;
            {
                int cntyplus = 0, cntyminus = 0;
                int cntxplus = 0, cntxminus = 0;
                int curr = i;
                int next = (curr + 1) % n_city;
                while (true) {
                    auto [x1, y1] = decode(path[curr]);
                    auto [x2, y2] = decode(path[next]);
                    cntxplus += x2 > x2;
                    cntxminus += x2 < x1;
                    if (x2 < 128) {
                        penalty++;
                        // cout << "x < 0 pen" << endl;
                        break;
                    }
                    if ((cntyplus + cntyminus) * 2 < cntxplus) {
                        penalty++;
                        // cout << "x > 2*y pen" << endl;
                        break;
                    }
                    cntyplus += y2 > y1;
                    cntyminus += y2 > y1;
                    if (cntyplus >= 64 && cntyminus >= 64)
                        break;
                    curr = (curr + 1) % n_city;
                    next = (next + 1) % n_city;
                }
            }
            {
                int cntyplus = 0, cntyminus = 0;
                int cntxplus = 0, cntxminus = 0;
                int curr = i;
                int next = (curr + n_city - 1) % n_city;
                while (true) {
                    auto [x1, y1] = decode(path[curr]);
                    auto [x2, y2] = decode(path[next]);
                    cntxplus += x2 > x2;
                    cntxminus += x2 < x1;
                    if (x2 < 128) {
                        penalty++;
                        // cout << "x < 0 pen" << endl;
                        break;
                    }
                    if ((cntyplus + cntyminus) * 2 < cntxplus) {
                        penalty++;
                        // cout << "x > 2*y pen" << endl;
                        break;
                    }
                    cntyplus += y2 > y1;
                    cntyminus += y2 > y1;
                    if (cntyplus >= 64 && cntyminus >= 64)
                        break;
                    curr = (curr + n_city - 1) % n_city;
                    next = (next + n_city - 1) % n_city;
                }
            }
        } else if (path[i] == corner1) {
            cnt++;
            {
                bool okx = false, oky = false;
                int curr = i;
                for (int j = 0; j < 255; ++j) {
                    auto [x1, y1] = decode(path[curr]);
                    if (x1 != 1)
                        okx = true;
                    if (y1 != 1)
                        oky = true;
                    curr = (curr + 1) % n_city;
                }
                if (!okx || !oky) {
                    penalty++;
                    // cout << "1, 1 pen" << endl;
                }
            }
            {
                bool okx = false, oky = false;
                int curr = i;
                for (int j = 0; j < 255; ++j) {
                    auto [x1, y1] = decode(path[curr]);
                    if (x1 != 1)
                        okx = true;
                    if (y1 != 1)
                        oky = true;
                    curr = (curr + n_city - 1) % n_city;
                }
                if (!okx || !oky) {
                    penalty++;
                    // cout << "1, 1 pen" << endl;
                }
            }

        } else if (path[i] == corner2) {
            cnt++;
            {
                bool okx = false, oky = false;
                int curr = i;
                for (int j = 0; j < 254; ++j) {
                    auto [x1, y1] = decode(path[curr]);
                    if (x1 != 255)
                        okx = true;
                    if (y1 != 255)
                        oky = true;
                    curr = (curr + 1) % n_city;
                }
                if (!okx || !oky) {
                    penalty++;
                    // cout << "255, 255 pen" << endl;
                }
            }
            {
                bool okx = false, oky = false;
                int curr = i;
                for (int j = 0; j < 254; ++j) {
                    auto [x1, y1] = decode(path[curr]);
                    if (x1 != 255)
                        okx = true;
                    if (y1 != 255)
                        oky = true;
                    curr = (curr + n_city - 1) % n_city;
                }
                if (!okx || !oky) {
                    penalty++;
                    // cout << "255, 255 pen" << endl;
                }
            }
        }
        if (cnt == 3)
            break;
    }
    for (int i = 0; i < 2; ++i) {
        int xnow, xbef, ynow, ybef;
        int now, bef;
        if (i == 0) {
            xnow = 128;
            xbef = 128 + 1;
            ynow = 128 - 4;
            ybef = 128 - 4;
        } else if (i == 1) {
            xnow = 128;
            xbef = 128 + 1;
            ynow = 128 - 5;
            ybef = 128 - 5;
        } else {
            assert(false);
        }
        now = encode(xnow, ynow);
        bef = encode(xbef, ybef);
        int inow = -1;
        int ibef = -1;
        for (int j = 0; j < n_city; ++j) {
            if (path[j] == now)
                inow = j;
            if (path[j] == bef)
                ibef = j;
        }
        if (abs(inow - ibef) != 1) {
            continue;
        }
        int dir = inow - ibef;
        int cntyplus = 0;
        vector<bool> isinquad(5, false);
        for (int j = 0; j < 2000; j++) {
            ibef = inow;
            inow = (inow + n_city + dir) % n_city;
            now = path[inow];
            bef = path[ibef];
            auto [xnow, ynow] = decode(now);
            auto [xbef, ybef] = decode(bef);
            int nowquad = quadrant(now);
            isinquad[nowquad] = true;

            if (isinquad[4])
                break;
            if (isinquad[1]) {
                if (isinquad[2] || isinquad[3])
                    penalty++;
                break;
            }
            cntyplus += ynow > ybef;
            if (cntyplus >= 128)
                break;
        }
    }
    // vector<vector<int>> quadcnt(n_city + 300, vector<int>(5, 0));
    // vector<int> yplus(n_city + 300, 0);
    // vector<int> yminus(n_city + 300, 0);
    // vector<int> xplus(n_city + 300, 0);
    // vector<int> yminus(n_city + 300, 0);
    // for (int i = 0; i < n_city; i++) {
    //     int q = quadrant(path[i]);
    //     quadcnt[i][q]++;
    //     quadcnt[i + 256][q]--;
    //     if (i != 0) {
    //         auto [cx, cy] = decode(path[i]);
    //         auto [px, py] = decode(path[i - 1]);
    // 		yplus[i] = cy - py > 0;
    // 		yminus[i] = cy - py < 0;
    // 		xplus[i] = cx - px > 0;
    // 		xminus[i] = cx - px > 0;
    //     }
    // }
    // for (int i = 0; i < n_city + 256; i++) {
    //     int qs = 0;
    //     for (int q = 1; q < 5; ++q) {
    //         quadcnt[i + 1][q] += quadcnt[i][q];
    //         if (quadcnt[i + 1][q] > 0)
    //             qs++;
    //     }
    //     if (qs == 4) {
    //         penalty++;
    //         cout << "quad pen at " << i << endl;
    //         break;
    //     }
    // }

    return penalty;
}

long double totalCost(vector<int>& path) {
    long double cost = 0.0;
    for (int i = 0; i < path.size() - 1; i++) {
        cost += funcEdgeDis(path[i], path[i + 1]);
    }
    cost += funcEdgeDis(path[path.size() - 1], path[0]);
    cost += penalty(path);
    return cost;
}

void write_path_to_file(string file, vector<int>& path) {
    ofstream f;
    f.open(file, ios::app);
    f << path.size() << " ";
    long double cost = totalCost(path);
    f << setprecision(15) << cost << endl;
    for (auto const& p : path) {
        f << p + 1 << " ";
    }
    f << endl;
    f.close();
}

int main(int argc, char* argv[]) {
    if (argc < 6) {
        cout << "./combine_pop.out ../input/image.csv temperature npop nsearch file1 [file2] ..." << endl;
        exit(-1);
    }
    char* imagepath = argv[1];
    long double temperature = atof(argv[2]);
    int npop = atoi(argv[3]);
    int nsearch = atoi(argv[4]);

    FILE* fp;
    fp = fopen(imagepath, "r");

    int x, y;
    double r, g, b;
    char _[50];
    fscanf(fp, "%s", _);
    for (int i = 0; i < n_city; i++) {
        fscanf(fp, "%d,%d,%lf,%lf,%lf", &x, &y, &r, &g, &b);
        x += center;
        y += center;
        image[x][y] = {r, g, b};
    }
    fclose(fp);

    for (int x1 = 0; x1 < len; x1++) {
        for (int y1 = 0; y1 < len; y1++) {
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
                    fEdgeDis[idx1][idx2] = sqrt(abs(dx) + abs(dy)) + 3.0 * (fabs(image[x1][y1][0] - image[x2][y2][0]) + fabs(image[x1][y1][1] - image[x2][y2][1]) + fabs(image[x1][y1][2] - image[x2][y2][2]));
                }
            }
        }
    }


    vector<pair<long double, vector<int>>> pop;
    over_npop = 1.0 / (long double)npop;

    for (int i = 5; i < argc; i++) {
        string file_path = argv[i];
        ifstream ifs(file_path);
        string str_buf;
        if (ifs.fail()) {
            cerr << "failed to open population file " << file_path << endl;
            cerr << "ignore population file and continue program" << endl;
            continue;
        }
        int cnt = 0;
        while (getline(ifs, str_buf)) {
            cnt++;
            if (str_buf == "")
                break;
            getline(ifs, str_buf);
            vector<int> route = string_to_vector_int(str_buf, ' ');
            assert(route.size() == n_city);
            long double cost = totalCost(route);
            // cerr << cnt << " " << cost << endl;


            if (pop.size() < npop) {  // simply add new route
                average_cost += cost * over_npop;
                pop.emplace_back(cost, route);
                addEdgeAll(route);
            } else {  // greedy, choose subset with smallest F = E - TH
                long double smallest_f = average_cost - temperature * entropy;
                int best_idx = -1;
                average_cost += cost * over_npop;
                addEdgeAll(route);
                // for (int l = 0; l < npop; l++) {
                for (int z = 0; z < nsearch; ++z) {
                    int l = rnd(npop);
                    removeEdgeAll(pop[l].second);
                    average_cost -= pop[l].first * over_npop;
                    long double f = average_cost - temperature * entropy;
                    // cerr << l << " " << setprecision(10) << average_cost << " " <<  entropy << endl;
                    if (f < smallest_f) {
                        smallest_f = f;
                        best_idx = l;
                    }
                    addEdgeAll(pop[l].second);
                    average_cost += pop[l].first * over_npop;
                }
                // cerr << best_idx << endl;
                if (best_idx == -1) {
                    removeEdgeAll(route);
                    average_cost -= (long double)cost * over_npop;
                    // continue;
                } else {
                    average_cost -= pop[best_idx].first * over_npop;
                    removeEdgeAll(pop[best_idx].second);
                    pair<long double, vector<int>> newpath = {cost, route};
                    pop[best_idx] = newpath;
                }
            }
            cerr << cnt << " " << setprecision(15) << average_cost << " " << average_cost - temperature * entropy << endl;
        }
    }
    cerr << "write results to file.." << endl;
    for (int i = 0; i < npop; i++) {
        write_path_to_file("./initial.txt", pop[i].second);
    }
    cerr << "completed." << endl;
    return 0;
}