#include <bits/stdc++.h>
using namespace std;
using lint         = long long;
constexpr int inf  = 1 << 29;
constexpr lint mod = 1000000007;

auto start = chrono::system_clock::now();
string gettime() {
    int elapsed = (int)(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start).count()) / 1000;
    return "elapsed time: " + to_string(elapsed);
}

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

XorShift rnd;

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
        if(di == u) {
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
    links.push_back(Link(64, hash % (1LL << 9LL)));
    hash >>= 9LL;
    links.push_back(Link(32, hash % (1LL << 8LL)));
    hash >>= 8LL;
    links.push_back(Link(16, hash % (1LL << 7LL)));
    hash >>= 7LL;
    links.push_back(Link(8, hash % (1LL << 6LL)));
    hash >>= 6LL;
    links.push_back(Link(4, hash % (1LL << 5LL)));
    hash >>= 5LL;
    links.push_back(Link(2, hash % (1LL << 4LL)));
    hash >>= 4LL;
    links.push_back(Link(1, hash % (1LL << 3LL)));
    hash >>= 3LL;
    links.push_back(Link(1, hash % (1LL << 3LL)));

    return links;
}

int corners(vector<Link> & links) {
    int res = 0;
    for(auto & l: links) {
        res += l.is_corner();
    }
    return res;
}

// for hash
vector<vector<int>> get_config(vector<Link>& links) {
    vector<vector<int>> configs;
    for (auto& l: links) {
        configs.push_back({l.x, l.y});
    }
    return configs;
}

void recursive_comb(int *indexes, int s, int rest, std::function<void(int *)> f) {
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

void foreach_comb(int n, int k, std::function<void(int *)> f) {
    int indexes[k];
    recursive_comb(indexes, n-1, k, f);
}

vector<vector<vector<vector<int>>>> combs(9, vector<vector<vector<int>>>(9, vector<vector<int>>()));

lint global_idx = 0;
vector<int> xuse = {0, 1, 2, 3, 4, 5, 6, 7};
vector<int> yuse = {0, 1, 2, 3, 4, 5, 6, 7};

int main() {
    // std::srand(std::time(0));
    // compute nCk combinations to list possible arm moves
    for (int i = 1; i <= 8; ++i) {
        for (int j = 1; j <= i; ++j) {
            foreach_comb(i, j, [&](int *indexes) {
                vector<int> comb;
                for (int k = 0; k < j; ++k) {
                    comb.push_back(indexes[k]);
                }
                combs[i][j].push_back(comb);
            });
            random_shuffle(combs[i][j].begin(), combs[i][j].end());
            // reverse(combs[i][j].begin(), combs[i][j].end());
        }
    }    

    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    vector<Link> links;
    links.push_back(Link(64));
    links.push_back(Link(32));
    links.push_back(Link(16));
    links.push_back(Link(8));
    links.push_back(Link(4));
    links.push_back(Link(2));
    links.push_back(Link(1));
    links.push_back(Link(1));

    int n; // length of path
    cin >> n;

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
    for (int i = 0; i < 64*8; i++) {
        links64.push_back(Link(64, i));
    }
    vector<Link> links32;
    for (int i = 0; i < 32*8; i++) {
        links32.push_back(Link(32, i));
    }
    vector<Link> links16;
    for (int i = 0; i < 16*8; i++) {
        links16.push_back(Link(16, i));
    }
    vector<Link> links8;
    for (int i = 0; i < 8*8; i++) {
        links8.push_back(Link(8, i));
    }
    vector<Link> links4;
    for (int i = 0; i < 4*8; i++) {
        links4.push_back(Link(4, i));
    }
    vector<vector<vector<bool>>> ok6432(n, vector<vector<bool>>(64*8, vector<bool>(32*8, false)));
    for (int i = 0; i < n; i++) {
        auto pos = pos_list[i];
        for (int j = 0; j < 64 * 8; j++) {
            for (int k = 0; k < 32 * 8; ++k) {
                // cerr << j << " " << links64[j].x << " " << links64[j].y << endl;
                int dx = pos[0] - links64[j].x - links32[k].x;
                int dy = pos[1] - links64[j].y - links32[k].y;
                if (abs(dx) <= 128-64-32 && abs(dy) <= 128-64-32)
                    ok6432[i][j][k] = true;
            }
        }        
    }
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j < 64 * 8; j++) {
            for (int k = 0; k < 32 * 8; k++) {
                ok6432[i][j][k] = ok6432[i][j][k] && 
                            (ok6432[i+1][j][k] || ok6432[i+1][(j+1)%(64*8)][k] || ok6432[i+1][(j+64*8-1)%(64*8)][k]
                            || ok6432[i+1][j][(k+1)%(32*8)] || ok6432[i+1][(j+1)%(64*8)][(k+1)%(32*8)] || ok6432[i+1][(j+64*8-1)%(64*8)][(k+1)%(32*8)]
                            || ok6432[i+1][j][(k+32*8-1)%(32*8)] || ok6432[i+1][(j+1)%(64*8)][(k+32*8-1)%(32*8)] || ok6432[i+1][(j+64*8-1)%(64*8)][(k+32*8-1)%(32*8)]
                            );
            }        
        }
    }

    vector<vector<bool>> ok16(n, vector<bool>(16*8, false));
    for (int i = 0; i < n; i++) {
        auto pos = pos_list[i];
        for (int j = 0; j < 16 * 8; j++) {
            // cerr << j << " " << links16[j].x << " " << links16[j].y << endl;
            int dx = pos[0] - links16[j].x;
            int dy = pos[1] - links16[j].y;
            if (abs(dx) <= 128-16 && abs(dy) <= 128-16)
                ok16[i][j] = true;
        }        
    }
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j < 16 * 8; j++) {
            ok16[i][j] = ok16[i][j] && (ok16[i+1][j] || ok16[i+1][(j+1)%(16*8)] || ok16[i+1][(j+16*8-1)%(16*8)]);
        }        
    }
    vector<vector<bool>> ok8(n, vector<bool>(8*8, false));
    for (int i = 0; i < n; i++) {
        auto pos = pos_list[i];
        for (int j = 0; j < 8 * 8; j++) {
            // cerr << j << " " << links8[j].x << " " << links8[j].y << endl;
            int dx = pos[0] - links8[j].x;
            int dy = pos[1] - links8[j].y;
            if (abs(dx) <= 128-8 && abs(dy) <= 128-8)
                ok8[i][j] = true;
        }        
    }
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j < 8 * 8; j++) {
            ok8[i][j] = ok8[i][j] && (ok8[i+1][j] || ok8[i+1][(j+1)%(8*8)] || ok8[i+1][(j+8*8-1)%(8*8)]);
        }        
    }
    vector<vector<bool>> ok4(n, vector<bool>(4*8, false));
    for (int i = 0; i < n; i++) {
        auto pos = pos_list[i];
        for (int j = 0; j < 4 * 8; j++) {
            // cerr << j << " " << links4[j].x << " " << links4[j].y << endl;
            int dx = pos[0] - links4[j].x;
            int dy = pos[1] - links4[j].y;
            if (abs(dx) <= 128-4 && abs(dy) <= 128-4)
                ok4[i][j] = true;
        }        
    }
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j < 4 * 8; j++) {
            ok4[i][j] = ok4[i][j] && (ok4[i+1][j] || ok4[i+1][(j+1)%(4*8)] || ok4[i+1][(j+4*8-1)%(4*8)]);
        }        
    }
    cerr << gettime() << ": start beam search" << endl;

    vector<vector<lint>> all_states;
    vector<vector<unsigned int>> all_prevs;
    vector<vector<Link>> states, states_nxt;
    states.push_back(links);
    set<vector<vector<int>>> counts, counts_nxt;
    counts.insert(get_config(links));

    all_states.push_back({get_links_hash(states[0])});
    all_prevs.push_back({(unsigned int)-1});

    // initial beam search parameters
    int maxi = 10000;
    int mult = 2;
    int restart = 256;
    int depth = 0;

    vector<int> width_decl(pos_list.size()*2, 0);

    for (int i = 0; i < (int) pos_list.size() - 1; ++i) {
        // states = all_states[all_states.size()-1];

        int dx = pos_list[i+1][0] - pos_list[i][0];
        int dy = pos_list[i+1][1] - pos_list[i][1];
        int absdx = abs(dx);
        vector<int> xmove = dx > 0 ? r : l;
        vector<int> xrev = dx > 0 ? l : r;
        int absdy = abs(dy);
        vector<int> ymove = dy > 0 ? u : d;
        vector<int> yrev = dy > 0 ? d : u;

        cerr << gettime() << ", step: " << i << ", pos: " << pos_list[i][0] << " " << pos_list[i][1] << ", dxdy: " << dx << " " << dy << ", num states: " << states.size() << endl;

        // revert increased beam width after passing over bottleneck point
        while(width_decl[i] < 0) {
            if (depth == 3) {
                restart -= 256;
            }
            maxi /= 2;
            depth--;
            width_decl[i]++;
        }

        int mx = min(maxi, (int)states.size());
        vector<unsigned int> prevs;
        set<int> chosen;
        for(int k = 0; k < mx; ++k) {
            int idx = rnd(min(mult*mx, (int)states.size())); // TODO: bug!!! not random choice for the long time, because this cant choose idx>mx=5000... this time it is sorted by corners so should be ok
            while(chosen.count(idx) == 1) 
                idx = rnd(mx);
            chosen.insert(idx);
            // int idx = k;
            vector<Link> c = states[idx];
            if (false) {
                int x = 0;
                int y = 0;
                for (auto& l: c) {
                    x += l.x;
                    y += l.y;
                }
                if (x != pos_list[i][0] || y != pos_list[i][1]) {
                    cerr << "pos unmatch: " << x << " " << y << ": " << pos_list[i][0] << " " << pos_list[i][1] << endl;
                    assert(false);
                }
            }

            if (dx != 0) {
                vector<int> xs;
                for (int k: xuse) {
                    if (c[k].can_move(xmove))
                        xs.push_back(k);
                }
                if ((int) xs.size() < absdx)
                    continue;

                for(auto const& indexes: combs[xs.size()][absdx]) {
                    for(int k = 0; k < absdx; ++k) {
                        c[xs[indexes[k]]].move(xmove);
                    }
                    if (dy != 0) {
                        vector<int> ys;
                        for (int k: yuse) {
                            if (c[k].can_move(ymove)) {
                                bool ok = true;
                                for (auto & xx: xs) {
                                    if (k == xx) {
                                        ok = false;
                                        break;
                                    }
                                }
                                if (ok)
                                    ys.push_back(k);
                            }
                        }
                        if ((int) ys.size() >= absdy) {
                            for(auto const& indexesy: combs[ys.size()][absdy]) {
                                for(int k = 0; k < absdy; ++k) {
                                    c[ys[indexesy[k]]].move(ymove);
                                }
                                auto cc = get_config(c);
                                if (counts_nxt.count(cc) == 0  && ok6432[i+1][c[0].get_hash()][c[1].get_hash()] && ok16[i+1][c[2].get_hash()]  && ok8[i+1][c[3].get_hash()] && ok4[i+1][c[4].get_hash()]) {
                                    prevs.push_back((unsigned int)idx);
                                    states_nxt.push_back(c);
                                    counts_nxt.insert(cc);
                                }
                                // revert y moves
                                for(int k = 0; k < absdy; ++k) {
                                    c[ys[indexesy[k]]].move(yrev);
                                }
                            }
                        }
                    } else {
                        auto cc = get_config(c);
                        if (counts_nxt.count(cc) == 0  && ok6432[i+1][c[0].get_hash()][c[1].get_hash()] && ok16[i+1][c[2].get_hash()]  && ok8[i+1][c[3].get_hash()] && ok4[i+1][c[4].get_hash()]) {
                            prevs.push_back((unsigned int)idx);
                            states_nxt.push_back(c);
                            counts_nxt.insert(cc);
                        }
                    }
                    // revert x moves
                    for(int k = 0; k < absdx; ++k) {
                        c[xs[indexes[k]]].move(xrev);
                    }
                }
            } else {
                vector<int> ys;
                for (int k: yuse) {
                    if (c[k].can_move(ymove))
                        ys.push_back(k);
                }
                if ((int) ys.size() >= absdy) {
                    for(auto const& indexesy: combs[ys.size()][absdy]) {
                        for(int k = 0; k < absdy; ++k) {
                            c[ys[indexesy[k]]].move(ymove);
                        }
                        auto cc = get_config(c);
                        if (counts_nxt.count(cc) == 0  && ok6432[i+1][c[0].get_hash()][c[1].get_hash()] && ok16[i+1][c[2].get_hash()]  && ok8[i+1][c[3].get_hash()] && ok4[i+1][c[4].get_hash()]) {
                            prevs.push_back((unsigned int)idx);
                            states_nxt.push_back(c);
                            counts_nxt.insert(cc);
                        }
                        for(int k = 0; k < absdy; ++k) {
                            c[ys[indexesy[k]]].move(yrev);
                        }
                    }
                } else {
                    continue;
                }
            }
        }

        if (states_nxt.size() == 0) {
            cerr << gettime() << ": no next candidates. Increase beam width. depth: " << depth << endl;
            if (depth < 5) {
                maxi *= 2;
                depth++;
                states.clear();
                states_nxt.clear();
                counts_nxt.clear();
                counts.clear();
                prevs.clear();
                if (depth == 3) {
                    restart += 256;
                }
                width_decl[i+restart+32]--;
            }
            int restart_use = min(restart, i);

            for (int z = 0; z < restart_use-1; ++z) {
                all_states.pop_back();
                all_prevs.pop_back();
            }
            for(auto& h: all_states.back()) {
                states.push_back(get_links(h));
            }
            i -= restart_use;
            continue;
        }
        vector<pair<int, pair<int, vector<Link>>>> states_nxt_sort;
        for (int z = 0; z < states_nxt.size(); ++z) {
            states_nxt_sort.emplace_back(-corners(states_nxt[z]), make_pair(prevs[z], states_nxt[z]));
        }
        sort(states_nxt_sort.begin(), states_nxt_sort.end(), [](auto const& lhs, auto const& rhs) {
            return lhs.first < rhs.first;
        });
        prevs.clear();
        states_nxt.clear();
        for (int z = 0; z < min(maxi*mult, (int) states_nxt_sort.size()); ++z) {
            prevs.push_back(states_nxt_sort[z].second.first);
            states_nxt.push_back(states_nxt_sort[z].second.second);
        }
        vector<lint> states_hist;
        for (auto& st: states_nxt) {
            states_hist.push_back(get_links_hash(st));
        }
        all_states.push_back(states_hist);
        all_prevs.push_back(prevs);
        swap(states, states_nxt);
        swap(counts, counts_nxt);
        states_nxt.clear();
        counts_nxt.clear();
        chosen.clear();
    }

    cerr << gettime() << ": completed!" << endl;
    int p = 0;
    for(int i = n - 1; i >= 0; --i) {
        lint state_hash = all_states[i][p];
        vector<Link> links = get_links(state_hash);

        // int x = 0;
        // int y = 0;
        // for (auto& l: links) {
        //     x += l.x;
        //     y += l.y;
        // }
        // cout << x << " " << y << " ";

        for(auto rr: links)
            cout << rr.x << " " << rr.y << " ";
        cout << endl;
        p = all_prevs[i][p];
    }

    return 0;
}
