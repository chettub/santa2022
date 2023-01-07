/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * GPX2: Generalized Partition Crossover 2
 * Reference:  R. Tinos, D. Whitley, and G. Ochoa (2017). A new generalized partition crossover for 	*
 *		the traveling salesman problem: tunneling between local optima. arXiv.org					*
 *  Contact: Renato Tinos <rtinos@ffclrp.usp.br>
 *
 * gpx2 is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gpx2 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "defs.h"
#include "int128.h"
#include <cmath>
#include <cstdlib>
#include <string>

/******************************************************************************\
*				  	random initialization of the parents						 *
\******************************************************************************/
void rand_init(int* p1, int* p2) {
    int *v_in, gene;

    v_in = aloc_vectori(n_cities);

    for (gene = 0; gene < n_cities; gene++) {
        v_in[gene] = gene;
    }
    rand_perm(v_in, p1, n_cities);  // random permutation of v_in
    rand_perm(v_in, p2, n_cities);  // random permutation of v_in

    delete[] v_in;
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

int128 penalty(vector<int>& path) {
    int center = encode(128, 128);
    int corner1 = encode(1, 1);
    int corner2 = encode(255, 255);

    int128 penalty = 0;

    int cnt = 0;
    for (int i = 0; i < path.size(); i++) {
        if (path[i] == center) {
            cnt++;
            {
                int cntyplus = 0, cntyminus = 0;
                int cntxplus = 0, cntxminus = 0;
                int curr = i;
                int next = (curr + 1) % n_cities;
                while (true) {
                    auto [x1, y1] = decode(path[curr]);
                    auto [x2, y2] = decode(path[next]);
                    cntxplus += x2 > x2;
                    cntxminus += x2 < x1;
                    if (x2 < 128) {
                        penalty++;
                        break;
                    }
                    if ((cntyplus + cntyminus) * 2 < cntxplus) {
                        penalty++;
                        break;
                    }
                    cntyplus += y2 > y1;
                    cntyminus += y2 > y1;
                    if (cntyplus >= 64 && cntyminus >= 64)
                        break;
                    curr = (curr + 1) % n_cities;
                    next = (next + 1) % n_cities;
                }
            }
            {
                int cntyplus = 0, cntyminus = 0;
                int cntxplus = 0, cntxminus = 0;
                int curr = i;
                int next = (curr + n_cities - 1) % n_cities;
                while (true) {
                    auto [x1, y1] = decode(path[curr]);
                    auto [x2, y2] = decode(path[next]);
                    cntxplus += x2 > x2;
                    cntxminus += x2 < x1;
                    if (x2 < 128) {
                        penalty++;
                        break;
                    }
                    if ((cntyplus + cntyminus) * 2 < cntxplus) {
                        penalty++;
                        break;
                    }
                    cntyplus += y2 > y1;
                    cntyminus += y2 > y1;
                    if (cntyplus >= 64 && cntyminus >= 64)
                        break;
                    curr = (curr + n_cities - 1) % n_cities;
                    next = (next + n_cities - 1) % n_cities;
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
                    curr = (curr + 1) % n_cities;
                }
                if (!okx || !oky)
                    penalty++;
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
                    curr = (curr + n_cities - 1) % n_cities;
                }
                if (!okx || !oky)
                    penalty++;
            }

        } else if (path[i] == corner2) {
            cnt++;
            {
                bool okx = false, oky = false;
                int curr = i;
                for (int j = 0; j < 255; ++j) {
                    auto [x1, y1] = decode(path[curr]);
                    if (x1 != 255)
                        okx = true;
                    if (y1 != 255)
                        oky = true;
                    curr = (curr + 1) % n_cities;
                }
                if (!okx || !oky)
                    penalty++;
            }
            {
                bool okx = false, oky = false;
                int curr = i;
                for (int j = 0; j < 255; ++j) {
                    auto [x1, y1] = decode(path[curr]);
                    if (x1 != 255)
                        okx = true;
                    if (y1 != 255)
                        oky = true;
                    curr = (curr + n_cities - 1) % n_cities;
                }
                if (!okx || !oky)
                    penalty++;
            }
        }
        if (cnt == 3)
            break;
    }
    vector<vector<int>> quadcnt(n_cities + 300, vector<int>(4, 0));
    for (int i = 0; i < n_cities; i++) {
        int q = quadrant(path[i]);
        quadcnt[i][q]++;
        quadcnt[i + 257][q]--;
    }
    for (int i = 0; i < n_cities; i++) {
		int qs = 0;
        for (int q = 0; q < 4; ++q) {
            quadcnt[i + 1][q] += quadcnt[i][q];
			if (quadcnt[i + 1][q] > 0)
				qs++;
        }
		if (qs == 4) {
			penalty++;
			break;
		}
    }

    return penalty * Magnification;
}

int128 total_cost(vector<int>& path) {
    int128 cost = 0;
    for (size_t i = 0; i < path.size() - 1; i++) {
        cost += weight(path[i], path[i + 1]);
    }
    cost += weight(path[path.size() - 1], path[0]);
    cost += penalty(path);
    return cost;
}

void write_path_to_file(string file, vector<int>& path) {
    ofstream f;
    f.open(file, ios::app);
    f << path.size() << " ";
    int128 cost = total_cost(path);
    f << cost << endl;
    for (auto const& p : path) {
        f << p + 1 << " ";
    }
    f << endl;
    f.close();
}


/******************************************************************************\
*				  	Main													 *
\******************************************************************************/
int main(int argc, char* argv[]) {
    int *p1, *p2, *offspring;
    int128 cost;
    string init_tour_file;

    // Arguments
    if (argc < 3) {
        cout << "Insufficient number of arguments!" << endl;
        exit(1);
    } else {
        prob_name = argv[1];
        init_tour_file = argv[2];
    }

    // TSP

    cout << "read problem from file " << prob_name << endl;

    read_problem(prob_name, init_tour_file);

    int n_cities = 257 * 257;

    cout << "\n ***** Example: recombination of all paths from file ****" << endl;
	cout << "Magnification " << Magnification << endl;
    vector<int> p3(n_cities, 1);
    vector<int> p4(n_cities, 1);

    vector<vector<int>> paths = readPop(init_tour_file);
    p3 = paths[0];
    vector<int> offspring2(n_cities, 0);
    // rand_init(p1, p2);  // random initialization of the parents
    string best_cost_str = "7407570654169005380529794198251561821613";
    int128 best_cost = parse(best_cost_str);
    // Recombination by GPX2
    for (int ii = 0; ii < paths.size(); ii++) {
        for (int i = ii + 1; i < paths.size() - 1; ++i) {
            p3 = paths[ii];
            p4 = paths[i];
            cout << "init cost1      : " << total_cost(p3) << endl;
            cout << "init cost2      : " << total_cost(p4) << endl;
            cost = gpx(p3, p4, offspring2);
            cout << "new cost (raw)  : " << cost << endl;
            int128 cost_restrictions = total_cost(offspring2);
            cout << "new cost        : " << cost_restrictions << endl;
            p3 = offspring2;
            if (cost < best_cost) {
                cout << "best score updated !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! save results." << endl;
                // write_path_to_file("./output/" + to_string_int128(cost) + ".txt", offspring2);
                write_path_to_file("./output/best.txt", offspring2);
                best_cost = cost;
            }
        }
    }

    // if (n_cities <= 15000)
    //     desaloc_matrixi(W, n_cities);
    delete[] coord_x;
    delete[] coord_y;
    // delete[] p1;
    // delete[] p2;
    // delete[] offspring;

    return 0;
}
