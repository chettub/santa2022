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

    if (argc != 2) {
        cerr << "./beam-parallel.out n_threads" << endl;
        exit(-1);
    }
    int n_thread = atoi(argv[1]);

    // compute nCk combinations to list possible arm moves
    for (int i = 1; i <= 8; ++i) {
        for (int j = 1; j <= i; ++j) {
            foreach_comb(i, j, [&](int* indexes) {
                vector<int> comb;
                for (int k = 0; k < j; ++k) {
                    comb.emplace_back(indexes[k]);
                }
                combs[i][j].emplace_back(comb);
            });
            random_shuffle(combs[i][j].begin(), combs[i][j].end());
        }
    }

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
    }

    cerr << gettime() << ": compute possible 64+16 positions by DP" << endl;

    vector<vector<vector<bool>>> ok6416(n, vector<vector<bool>>(64 * 8, vector<bool>(16 * 8, false)));
    for (int i = 0; i < n; i++) {
        auto pos = pos_list[i];
        for (int j = 0; j < 64 * 8; j++) {
            for (int k = 0; k < 16 * 8; ++k) {
                // cerr << j << " " << links64[j].x << " " << links64[j].y << endl;
                int dx = pos[0] - links64[j].x - links16[k].x;
                int dy = pos[1] - links64[j].y - links16[k].y;
                if (16 <= max(abs(dx), abs(dy)) && max(abs(dx), abs(dy)) <= 48)
                    ok6416[i][j][k] = true;
            }
        }
    }
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j < 64 * 8; j++) {
            for (int k = 0; k < 16 * 8; k++) {
                ok6416[i][j][k] = ok6416[i][j][k] && (ok6416[i + 1][j][k] || ok6416[i + 1][(j + 1) % (64 * 8)][k] || ok6416[i + 1][(j + 64 * 8 - 1) % (64 * 8)][k] || ok6416[i + 1][j][(k + 1) % (16 * 8)] || ok6416[i + 1][(j + 1) % (64 * 8)][(k + 1) % (16 * 8)] || ok6416[i + 1][(j + 64 * 8 - 1) % (64 * 8)][(k + 1) % (16 * 8)] || ok6416[i + 1][j][(k + 16 * 8 - 1) % (16 * 8)] || ok6416[i + 1][(j + 1) % (64 * 8)][(k + 16 * 8 - 1) % (16 * 8)] || ok6416[i + 1][(j + 64 * 8 - 1) % (64 * 8)][(k + 16 * 8 - 1) % (16 * 8)]);
            }
        }
    }

    vector<vector<bool>> ok8(n, vector<bool>(8 * 8, false));
    for (int i = 0; i < n; i++) {
        auto pos = pos_list[i];
        for (int j = 0; j < 8 * 8; j++) {
            // cerr << j << " " << links8[j].x << " " << links8[j].y << endl;
            int dx = pos[0] - links8[j].x;
            int dy = pos[1] - links8[j].y;
            if (abs(dx) <= 128 - 8 && abs(dy) <= 128 - 8)
                ok8[i][j] = true;
        }
    }
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j < 8 * 8; j++) {
            ok8[i][j] = ok8[i][j] && (ok8[i + 1][j] || ok8[i + 1][(j + 1) % (8 * 8)] || ok8[i + 1][(j + 8 * 8 - 1) % (8 * 8)]);
        }
    }
    vector<vector<bool>> ok4(n, vector<bool>(4 * 8, false));
    for (int i = 0; i < n; i++) {
        auto pos = pos_list[i];
        for (int j = 0; j < 4 * 8; j++) {
            // cerr << j << " " << links4[j].x << " " << links4[j].y << endl;
            int dx = pos[0] - links4[j].x;
            int dy = pos[1] - links4[j].y;
            if (abs(dx) <= 128 - 4 && abs(dy) <= 128 - 4)
                ok4[i][j] = true;
        }
    }
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j < 4 * 8; j++) {
            ok4[i][j] = ok4[i][j] && (ok4[i + 1][j] || ok4[i + 1][(j + 1) % (4 * 8)] || ok4[i + 1][(j + 4 * 8 - 1) % (4 * 8)]);
        }
    }
    cerr << gettime() << ": start beam search" << endl;

    vector<vector<pair<lint, int>>> all_states;
    vector<pair<vector<Link>, int>> states, states_nxt_main;
    states.push_back({links, -1});
    // set<vector<vector<int>>> counts_nxt_main;
    unordered_set<lint> counts_nxt_main_hash;
    all_states.push_back({make_pair<lint, int>(get_links_hash(states[0].first), -1)});

    // initial beam search parameters ----------------------------------------
    int maxi = 500;     // beam width, how many states to keep
    int mult = 2;       // beam width multiplier, increase beam width by this when failed
    int restart = 256;  // restart steps, go back to previous steps when failed
    int max_depth = 6;  // maximum restart depth. stop increasing beam width here
    int depth = 0;      // current restart depth. there're maximum depth
    // initial beam search parameters ----------------------------------------

    vector<int> width_decl(pos_list.size() * 2, 0);

    for (int i = 0; i < (int)pos_list.size() - 1; ++i) {
        // states = all_states[all_states.size()-1];

        int dx = pos_list[i + 1][0] - pos_list[i][0];
        int dy = pos_list[i + 1][1] - pos_list[i][1];
        int absdx = abs(dx);
        vector<int> xmove = dx > 0 ? r : l;
        vector<int> xrev = dx > 0 ? l : r;
        int absdy = abs(dy);
        vector<int> ymove = dy > 0 ? u : d;
        vector<int> yrev = dy > 0 ? d : u;

        cerr << gettime() << ", step: " << i << ", pos: " << pos_list[i][0] << " " << pos_list[i][1] << ", dxdy: " << dx << " " << dy << ", num states: " << states.size() << endl;

        // revert increased beam width after passing over bottleneck point
        while (width_decl[i] < 0) {
            if (depth == 3) {
                restart -= 256;
            }
            maxi /= 2;
            depth--;
            width_decl[i]++;
        }

        int mx = min(maxi, (int)states.size());

        vector<int> job_idx(n_thread + 1, mx / n_thread);
        job_idx[0] = 0;
        for (int j = 1; j <= mx % n_thread; ++j) {
            job_idx[j]++;
        }
        for (int j = 0; j < (int)job_idx.size() - 1; ++j) {
            job_idx[j + 1] += job_idx[j];
        }

        vector<thread> threads;
        for (int ith = 0; ith < n_thread; ++ith) {
            thread th([&](int ii) {
                vector<pair<vector<Link>, int>> states_nxt;
                // set<vector<vector<int>>> counts_nxt;
                unordered_set<lint> counts_nxt_hash;

                for (int idx = job_idx[ii]; idx < job_idx[ii + 1]; ++idx) {
                    vector<Link> c = states[idx].first;

                    // sanity check for debugging
                    if (false) {
                        int x = 0;
                        int y = 0;
                        for (auto& l : c) {
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
                        for (int k : xuse) {
                            if (c[k].can_move(xmove))
                                xs.emplace_back(k);
                        }
                        if ((int)xs.size() < absdx)
                            continue;

                        for (auto const& indexes : combs[xs.size()][absdx]) {
                            for (int k = 0; k < absdx; ++k) {
                                c[xs[indexes[k]]].move(xmove);
                            }
                            if (dy != 0) {
                                vector<int> ys;
                                for (int k : yuse) {
                                    if (c[k].can_move(ymove)) {
                                        bool ok = true;
                                        for (auto& xx : xs) {
                                            if (k == xx) {
                                                ok = false;
                                                break;
                                            }
                                        }
                                        if (ok)
                                            ys.emplace_back(k);
                                    }
                                }
                                if ((int)ys.size() >= absdy) {
                                    for (auto const& indexesy : combs[ys.size()][absdy]) {
                                        for (int k = 0; k < absdy; ++k) {
                                            c[ys[indexesy[k]]].move(ymove);
                                        }
                                        // auto cc = get_config(c);
                                        auto cc_hash = get_links_hash(c);
                                        if (ok6432[i + 1][c[0].get_hash()][c[1].get_hash()] && ok6416[i + 1][c[0].get_hash()][c[2].get_hash()] && ok8[i + 1][c[3].get_hash()] && ok4[i + 1][c[4].get_hash()]) {
                                            auto [it, flag] = counts_nxt_hash.insert(cc_hash);
                                            if (flag)
                                                states_nxt.emplace_back(c, idx);
                                        }
                                        // revert y moves
                                        for (int k = 0; k < absdy; ++k) {
                                            c[ys[indexesy[k]]].move(yrev);
                                        }
                                    }
                                }
                            } else {
                                // auto cc = get_config(c);
                                auto cc_hash = get_links_hash(c);
                                if (ok6432[i + 1][c[0].get_hash()][c[1].get_hash()] && ok6416[i + 1][c[0].get_hash()][c[2].get_hash()] && ok8[i + 1][c[3].get_hash()] && ok4[i + 1][c[4].get_hash()]) {
                                    auto [it, flag] = counts_nxt_hash.insert(cc_hash);
                                    if (flag)
                                        states_nxt.emplace_back(c, idx);
                                }
                            }
                            // revert x moves
                            for (int k = 0; k < absdx; ++k) {
                                c[xs[indexes[k]]].move(xrev);
                            }
                        }
                    } else {
                        vector<int> ys;
                        for (int k : yuse) {
                            if (c[k].can_move(ymove))
                                ys.emplace_back(k);
                        }
                        if ((int)ys.size() >= absdy) {
                            for (auto const& indexesy : combs[ys.size()][absdy]) {
                                for (int k = 0; k < absdy; ++k) {
                                    c[ys[indexesy[k]]].move(ymove);
                                }
                                // auto cc = get_config(c);
                                auto cc_hash = get_links_hash(c);
                                if (ok6432[i + 1][c[0].get_hash()][c[1].get_hash()] && ok6416[i + 1][c[0].get_hash()][c[2].get_hash()] && ok8[i + 1][c[3].get_hash()] && ok4[i + 1][c[4].get_hash()]) {
                                    auto [it, flag] = counts_nxt_hash.insert(cc_hash);
                                    if (flag)
                                        states_nxt.emplace_back(c, idx);
                                }
                                for (int k = 0; k < absdy; ++k) {
                                    c[ys[indexesy[k]]].move(yrev);
                                }
                            }
                        } else {
                            continue;
                        }
                    }
                }

                {
                    // obtain lock, append found states back to main thread
                    lock_guard<mutex> lock(mtx);
                    for (auto& p : states_nxt) {
                        // auto cc = get_config(p.first);
                        auto cc_hash = get_links_hash(p.first);
                        auto [it, flag] = counts_nxt_main_hash.insert(cc_hash);
                        if (flag)
                            states_nxt_main.emplace_back(p);
                    }
                }
            },
                ith);
            threads.emplace_back(move(th));
        }
        for (auto& th : threads) {
            th.join();
        }
        threads.clear();

        if (states_nxt_main.size() == 0) {
            cerr << gettime() << ": no next candidates. Increase beam width. depth: " << depth << endl;
            if (depth < max_depth) {
                maxi *= 2;
                depth++;
                states.clear();
                if (depth == 3) {
                    restart += 256;
                }
                width_decl[i + restart + 32]--;
            }
            int restart_use = min(restart, i);

            for (int z = 0; z < restart_use - 1; ++z) {
                all_states.pop_back();
            }
            for (auto& p : all_states.back()) {
                states.emplace_back(get_links(p.first), p.second);
            }
            i -= restart_use;
            continue;
        }

        vector<pair<int, pair<vector<Link>, int>>> states_nxt_sort;
        states_nxt_sort.reserve(states_nxt_main.size());
        for (int z = 0; z < states_nxt_main.size(); ++z) {
            states_nxt_sort.emplace_back(-corners(states_nxt_main[z].first), states_nxt_main[z]);
        }
        sort(states_nxt_sort.begin(), states_nxt_sort.end(), [](auto const& lhs, auto const& rhs) {
            return lhs.first < rhs.first;
        });
        states_nxt_main.clear();
        for (int z = 0; z < min(maxi * mult, (int)states_nxt_sort.size()); ++z) {
            states_nxt_main.emplace_back(states_nxt_sort[z].second);
        }

        random_shuffle(states_nxt_main.begin(), states_nxt_main.end());

        vector<pair<lint, int>> states_hist;
        states_hist.reserve(states_nxt_main.size());
        for (auto& st : states_nxt_main) {
            states_hist.emplace_back(get_links_hash(st.first), st.second);
        }
        all_states.emplace_back(states_hist);
        swap(states, states_nxt_main);
        states_nxt_main.clear();
        counts_nxt_main_hash.clear();
    }

    cerr << gettime() << ": search completed!" << endl;
    vector<vector<Link>> results(n);
    int p = 0;
    for (int i = n - 1; i >= 0; --i) {
        lint state_hash = all_states[i][p].first;
        results[i] = get_links(state_hash);

        for (auto rr : results[i])
            cout << rr.x << " " << rr.y << " ";
        cout << endl;
        p = all_states[i][p].second;
    }

    cerr << gettime() << ": verifying result..." << endl;
    vector<int> arm_length = {64, 32, 16, 8, 4, 2, 1, 1};
    for (int i = 0; i < n; ++i) {
        int x = 0;
        int y = 0;
        for (int j = 0; j < 8; ++j) {
            x += results[i][j].x;
            y += results[i][j].y;
        }
        if (x != pos_list[i][0] || y != pos_list[i][1]) {
            cerr << "result position unmatch at step " << i << endl;
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 8; ++j) {
            int len = max(abs(results[i][j].x), abs(results[i][j].y));
            if (len != arm_length[j])
                cerr << "config arm length mismatch at step " << i << endl;
        }
    }
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < 8; ++j) {
            int move = abs(results[i][j].x - results[i + 1][j].x) + abs(results[i][j].y - results[i + 1][j].y);
            if (move > 1)
                cerr << "config arm " << j << " moves too much at step " << i << endl;
        }
    }
    cerr << gettime() << ": completed." << endl;
    return 0;
}
