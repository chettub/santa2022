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

int128 total_cost(vector<int>& path) {
    int128 cost = 0;
    for (size_t i = 0; i < path.size() - 1; i++) {
        cost += weight(path[i], path[i + 1]);
    }
    cost += weight(path[path.size() - 1], path[0]);
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

    vector<int> p3(n_cities, 1);
    vector<int> p4(n_cities, 1);

    vector<vector<int>> paths = readPop(init_tour_file);
    p3 = paths[0];
    vector<int> offspring2(n_cities, 0);
    // rand_init(p1, p2);  // random initialization of the parents
    int128 best_cost = total_cost(p3);
    // Recombination by GPX2
    for (int i = 0; i < paths.size() - 1; ++i) {
        cout << "path: " << i + 1 << endl;
        p4 = paths[i + 1];
        cout << "init cost1: " << total_cost(p3) << endl;
        cout << "init cost2: " << total_cost(p4) << endl;
        cost = gpx(p3, p4, offspring2);
        cout << "new cost  : " << cost << endl;
        p3 = offspring2;
        if (cost < best_cost) {
            // write_path_to_file("./output/" + to_string_int128(cost) + ".txt", offspring2);
            write_path_to_file("./output/best.txt", offspring2);
            best_cost = cost;
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
