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

constexpr double inf = 100.0;
constexpr int len = 257;
constexpr int n_city = len * len;
constexpr int center = 128;

double average_cost = 0.0;
double entropy = 0.0;
vector<map<int, int>> edge_count(n_city);
double over_npop;
vector<vector<vector<double>>> image(len, vector<vector<double>>(len, vector<double>(3, 0.0)));
vector<vector<double>> fEdgeDis(n_city, vector<double>(17 * 17, inf));
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

inline double log(double x) {
    double xm1 = x - 1.0;
    return xm1 * (1.0 - xm1 * (0.5 - 0.333333 * xm1));
}

void addEdge(int i, int j) {
    int cnt = edge_count[i][j];
    double r = 0.0;
    if (cnt != 0) {
        r = double(cnt) * over_npop;
        entropy -= r * log(r);
    }
    edge_count[i][j]++;
    cnt++;
    r = double(cnt) * over_npop;
    entropy += r * log(r);
}

void removeEdge(int i, int j) {
    int cnt = edge_count[i][j];
    double r = 0.0;
    if (cnt != 0) {
        r = double(cnt) * over_npop;
        entropy -= r * log(r);
    }
    edge_count[i][j]--;
    assert(edge_count[i][j] >= 0);
    cnt--;
    if (cnt != 0) {
        r = double(cnt) * over_npop;
        entropy += r * log(r);
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

double funcEdgeDis(int i, int j) {
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

double totalCost(vector<int>& path) {
    double cost = 0.0;
    for (int i = 0; i < path.size() - 1; i++) {
        cost += funcEdgeDis(path[i], path[i + 1]);
    }
    cost += funcEdgeDis(path[path.size() - 1], path[0]);
    return cost;
}

void write_path_to_file(string file, vector<int>& path) {
    ofstream f;
    f.open(file, ios::app);
    f << path.size() << " ";
    double cost = totalCost(path);
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
    double temperature = atof(argv[2]);
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


    vector<pair<double, vector<int>>> pop;
    over_npop = 1.0 / (double)npop;

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
            double cost = totalCost(route);
            // cerr << cnt << " " << cost << endl;


            if (pop.size() < npop) {  // simply add new route
                average_cost += cost * over_npop;
                pop.emplace_back(cost, route);
                addEdgeAll(route);
            } else {  // greedy, choose subset with smallest F = E - TH
                double smallest_f = average_cost - temperature * entropy;
                int best_idx = -1;
                average_cost += cost * over_npop;
                addEdgeAll(route);
                // for (int l = 0; l < npop; l++) {
                for (int z = 0; z < n_search; ++z) {
                    int l = rnd(npop);
                    removeEdgeAll(pop[l].second);
                    average_cost -= pop[l].first * over_npop;
                    double f = average_cost - temperature * entropy;
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
                    average_cost -= (double)cost * over_npop;
                    // continue;
                } else {
                    average_cost -= pop[best_idx].first * over_npop;
                    removeEdgeAll(pop[best_idx].second);
                    pair<double, vector<int>> newpath = {cost, route};
                    pop[best_idx] = newpath;
                }
            }
            cerr << cnt << " " << setprecision(10) << average_cost << " " << average_cost - temperature * entropy << endl;
        }
    }
    cerr << "write results to file.." << endl;
    for (int i = 0; i < npop; i++) {
        write_path_to_file("./initial.txt", pop[i].second);
    }
    cerr << "completed." << endl;
    return 0;
}