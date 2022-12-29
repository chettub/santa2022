#include <bits/stdc++.h>
using namespace std;
using lint = long long;
constexpr int inf = 1 << 29;
constexpr lint mod = 1000000007;

mutex mtx;

auto start = chrono::system_clock::now();
string gettime() {
    int elapsed = (int)(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start).count()) / 1000;
    return "elapsed time: " + to_string(elapsed);
}

const vector<int> u = {0, 1};
const vector<int> d = {0, -1};
const vector<int> l = {-1, 0};
const vector<int> r = {1, 0};

struct Link {
    int n;
    int x, y;

    Link(int n_) : n(n_), x(-n_), y(0) {}
    Link(int n_, lint hash) : n(n_) {
        int div = hash / (2 * n);
        int rem = hash % (2 * n);
        if (div == 0) {
            x = -n;
            y = rem - n;
        } else if (div == 1) {
            y = n;
            x = rem - n;
        } else if (div == 2) {
            x = n;
            y = -rem + n;
        } else if (div == 3) {
            y = -n;
            x = -rem + n;
        }
        // cerr << "hash recov: " << n << " " << x << " " << y << " " << div << " " << rem << " " << hash << endl;
    }

    void set_pos(vector<int> link) {
        x = link[0];
        y = link[1];
    }

    lint get_hash() const {
        lint hash = -1;
        if (x == -n) {
            hash = y + n;
        } else if (y == n) {
            hash = x + 3 * n;
        } else if (x == n) {
            hash = -y + 5 * n;
        } else if (y == -n) {
            hash = -x + 7 * n;
        } else {
            cerr << n << " " << x << " " << y << endl;
            assert(false);
        }
        // cerr << n << " " << x << " " << y << " " << hash << endl;
        assert(hash >= 0);
        assert(hash < 8 * n);
        return hash;
    }

    bool can_move(vector<int> const& di) {
        if (di == u) {
            if ((x == -n || x == n) && (y != n))
                return true;
        } else if (di == d) {
            if ((x == -n || x == n) && (y != -n))
                return true;
        } else if (di == l) {
            if ((y == -n || y == n) && (x != -n))
                return true;
        } else if (di == r) {
            if ((y == -n || y == n) && (x != n))
                return true;
        }
        return false;
    }

    void move(vector<int> const& di) {
        assert(can_move(di));
        x += di[0];
        y += di[1];
    }

    int is_corner() {
        return n == abs(x) && n == abs(y);
    }
};

lint get_links_hash(vector<Link> const& links) {
    assert(links.size() == 8);
    lint res = 0LL;
    res += links[7].get_hash();
    res <<= 3LL;
    res += links[6].get_hash();
    res <<= 4LL;
    res += links[5].get_hash();
    res <<= 5LL;
    res += links[4].get_hash();
    res <<= 6LL;
    res += links[3].get_hash();
    res <<= 7LL;
    res += links[2].get_hash();
    res <<= 8LL;
    res += links[1].get_hash();
    res <<= 9LL;
    res += links[0].get_hash();
    return res;
}

vector<Link> get_links(lint hash) {
    vector<Link> links;
    links.reserve(8);
    links.emplace_back(Link(64, hash % (1LL << 9LL)));
    hash >>= 9LL;
    links.emplace_back(Link(32, hash % (1LL << 8LL)));
    hash >>= 8LL;
    links.emplace_back(Link(16, hash % (1LL << 7LL)));
    hash >>= 7LL;
    links.emplace_back(Link(8, hash % (1LL << 6LL)));
    hash >>= 6LL;
    links.emplace_back(Link(4, hash % (1LL << 5LL)));
    hash >>= 5LL;
    links.emplace_back(Link(2, hash % (1LL << 4LL)));
    hash >>= 4LL;
    links.emplace_back(Link(1, hash % (1LL << 3LL)));
    hash >>= 3LL;
    links.emplace_back(Link(1, hash % (1LL << 3LL)));

    return links;
}

int corners(vector<Link>& links) {
    int res = 0;
    for (auto& l : links) {
        res += l.is_corner();
    }
    return res;
}

// for hash
// vector<vector<int>> get_config(vector<Link>& links) {
//     vector<vector<int>> configs;
//     for (auto& l : links) {
//         configs.emplace_back({l.x, l.y});
//     }
//     return configs;
// }

void recursive_comb(int* indexes, int s, int rest, std::function<void(int*)> f) {
    if (rest == 0)
        f(indexes);
    else {
        if (s < 0)
            return;
        recursive_comb(indexes, s - 1, rest, f);
        indexes[rest - 1] = s;
        recursive_comb(indexes, s - 1, rest - 1, f);
    }
}

void foreach_comb(int n, int k, std::function<void(int*)> f) {
    int indexes[k];
    recursive_comb(indexes, n - 1, k, f);
}

vector<vector<vector<vector<int>>>> combs(9, vector<vector<vector<int>>>(9, vector<vector<int>>()));

lint global_idx = 0;
vector<int> xuse = {0, 1, 2, 3, 4, 5, 6, 7};
vector<int> yuse = {0, 1, 2, 3, 4, 5, 6, 7};

int main(int argc, char* argv[]) {

    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    vector<Link> links;
    links.emplace_back(Link(64));
    links.emplace_back(Link(32));
    links.emplace_back(Link(16));
    links.emplace_back(Link(8));
    links.emplace_back(Link(4));
    links.emplace_back(Link(2));
    links.emplace_back(Link(1));
    links.emplace_back(Link(1));

    int n;  // length of path
    cin >> n;

    // n = 500;  // for small test

    // initial config
    for (size_t i = 0; i < 8; i++) {
        cin >> links[i].x >> links[i].y;
    }

    // path
    vector<vector<int>> pos_list;
    for (int i = 0; i < n; i++) {
        int x, y;
        cin >> x >> y;
        pos_list.push_back({x, y});
    }

    cerr << gettime() << ": compute possible 64+32 positions by DP" << endl;

    vector<Link> links64;
    for (int i = 0; i < 64 * 8; i++) {
        links64.emplace_back(Link(64, i));
    }
    vector<Link> links32;
    for (int i = 0; i < 32 * 8; i++) {
        links32.emplace_back(Link(32, i));
    }
    vector<Link> links16;
    for (int i = 0; i < 16 * 8; i++) {
        links16.emplace_back(Link(16, i));
    }
    vector<Link> links8;
    for (int i = 0; i < 8 * 8; i++) {
        links8.emplace_back(Link(8, i));
    }
    vector<Link> links4;
    for (int i = 0; i < 4 * 8; i++) {
        links4.emplace_back(Link(4, i));
    }

    // for testing infeasible route
    // reverse(pos_list.begin(), pos_list.end());

    vector<vector<vector<bool>>> ok6432(n, vector<vector<bool>>(64 * 8, vector<bool>(32 * 8, false)));
    for (int i = 0; i < n; i++) {
        auto pos = pos_list[i];
        for (int j = 0; j < 64 * 8; j++) {
            for (int k = 0; k < 32 * 8; ++k) {
                // cerr << j << " " << links64[j].x << " " << links64[j].y << endl;
                int dx = pos[0] - links64[j].x - links32[k].x;
                int dy = pos[1] - links64[j].y - links32[k].y;
                if (abs(dx) <= 128 - 64 - 32 && abs(dy) <= 128 - 64 - 32)
                    ok6432[i][j][k] = true;
            }
        }
    }
    for (int i = n - 2; i >= 0; i--) {
        auto posnow = pos_list[i];
        auto posnxt = pos_list[i + 1];
        int ddxpos = posnxt[0] - posnow[0];
        int ddypos = posnxt[1] - posnow[1];
        for (int jnow = 0; jnow < 64 * 8; jnow++) {
            for (int know = 0; know < 32 * 8; know++) {
                // int j = jnow;
                // int k = know;
                // ok6432[i][j][k] = ok6432[i][j][k] && (ok6432[i + 1][j][k] || ok6432[i + 1][(j + 1) % (64 * 8)][k] || ok6432[i + 1][(j + 64 * 8 - 1) % (64 * 8)][k] || ok6432[i + 1][j][(k + 1) % (32 * 8)] || ok6432[i + 1][(j + 1) % (64 * 8)][(k + 1) % (32 * 8)] || ok6432[i + 1][(j + 64 * 8 - 1) % (64 * 8)][(k + 1) % (32 * 8)] || ok6432[i + 1][j][(k + 32 * 8 - 1) % (32 * 8)] || ok6432[i + 1][(j + 1) % (64 * 8)][(k + 32 * 8 - 1) % (32 * 8)] || ok6432[i + 1][(j + 64 * 8 - 1) % (64 * 8)][(k + 32 * 8 - 1) % (32 * 8)]);
                // continue;
                if (!ok6432[i][jnow][know])
                    continue;
                bool flag = false;
                Link& l64now = links64[jnow];
                Link& l32now = links32[know];
                for (int dj = -1; dj <= 1; dj++) {
                    for (int dk = -1; dk <= 1; dk++) {
                        int jnxt = (jnow + dj + 64 * 8) % (64 * 8);
                        int knxt = (know + dk + 32 * 8) % (32 * 8);
                        if (!ok6432[i + 1][jnxt][knxt])
                            continue;
                        Link& l64nxt = links64[jnxt];
                        Link& l32nxt = links32[knxt];
                        int ddxarm = l64nxt.x + l32nxt.x - l64now.x - l32now.x;
                        int ddyarm = l64nxt.y + l32nxt.y - l64now.y - l32now.y;
                        int cntmove = abs(dj) + abs(dk);
                        if (cntmove + abs(ddxpos - ddxarm) + abs(ddypos - ddyarm) == abs(ddxpos) + abs(ddypos))
                            flag = true;
                        if (flag)
                            break;
                    }
                    if (flag)
                        break;
                }
                ok6432[i][jnow][know] = flag;
            }
        }

        bool check_possible = false;
        for (int jnow = 0; jnow < 64 * 8; jnow++) {
            for (int know = 0; know < 32 * 8; know++) {
                if (ok6432[i][jnow][know]) {
                    check_possible = true;
                    break;
                }
            }
            if (check_possible)
                break;
        }
        if (!check_possible) {
            cerr << "cannot move arm 64,32 from " << i + 1 << " to " << i << endl;
            cerr << "coordinates : (" << posnxt[0] << "," << posnxt[1] << ") -> (" << posnow[0] << "," << posnow[1] << ")" << endl;
            exit(-1);
        }
    }
    cerr << "64-32 arms dp check passed." << endl;
    return 0;
}
