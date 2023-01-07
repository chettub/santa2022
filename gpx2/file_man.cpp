/******************************************************************************\
*				  				 Files Manipulation							 *
\******************************************************************************/

#include "defs.h"
#include "int128.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define CHAR_LEN 100

int128 INF = 100;
int magnification_digits = 25;
int128 Magnification = 1;
int128** fEdgeDis;

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

/****************************************************************/
/* Read the problem instance (adapted from DTSP generator - Yang et al.) */
/****************************************************************/
void read_problem(char* filename, string init_file_name) {
    for (size_t i = 0; i < magnification_digits; i++) {
        Magnification *= 10;
    }
    vector<int128> sqrt_int128(9, 0);
    string sqrt2 = "14142135623730950488016887";
    string sqrt3 = "17320508075688772935274463";
    string sqrt5 = "22360679774997896964091736";
    string sqrt6 = "24494897427831780981972840";
    string sqrt7 = "26457513110645905905016157";
    sqrt_int128[0] = 0;
    sqrt_int128[1] = Magnification;
    sqrt_int128[2] = parse(sqrt2);
    sqrt_int128[3] = parse(sqrt3);
    sqrt_int128[4] = 2 * Magnification;
    sqrt_int128[5] = parse(sqrt5);
    sqrt_int128[6] = parse(sqrt6);
    sqrt_int128[7] = parse(sqrt7);
    sqrt_int128[8] = 2 * sqrt_int128[2];

    int128 Len = 257;
    int128 Ncity = Len * Len;
    int128 Center = 128;
    vector<vector<vector<int128>>> Image(Len, vector<vector<int128>>(Len, vector<int128>(3, 0)));

    FILE* fp;
    fp = fopen(filename, "r");
	char _[20];
	fscanf(fp, "%s", _);  // discard first line
    int x, y;
    int r1, g1, b1;
    char r0[20], g0[20], b0[20];
    for (int i = 0; i < Ncity; i++) {
		// cout << "read line " << i << endl;
        fscanf(fp, "%d,%d,%d.%[^,],%d.%[^,],%d.%s", &x, &y, &r1, r0, &g1, g0, &b1, b0);
        x += Center;
        y += Center;
        string rs = to_string(r1);
        string gs = to_string(g1);
        string bs = to_string(b1);

		// cout << x << " " << y << endl;

        // cout << rs << " " << gs << " " << bs << endl;

        rs += r0;
        gs += g0;
        bs += b0;

        // cout << rs << " " << gs << " " << bs << endl;

        int128 r = parse(rs);
        for (int j = 0; j < magnification_digits - (rs.size() - 1); ++j) {
            r *= 10;
        }
        int128 g = parse(gs);
        for (int j = 0; j < magnification_digits - (gs.size() - 1); ++j) {
            g *= 10;
        }
        int128 b = parse(bs);
        for (int j = 0; j < magnification_digits - (bs.size() - 1); ++j) {
            b *= 10;
        }

        // cout << r << " " << g << " " << b << endl;

        Image[x][y] = {r, g, b};
    }

    fEdgeDis = new int128*[Ncity];
    for (int i = 0; i < Ncity; ++i) {
        fEdgeDis[i] = new int128[17 * 17];
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
                    int128 tmp = 0;
                    tmp += sqrt_int128[abs(dx) + abs(dy)];
                    tmp += int128(3) * (abs_int128(Image[x1][y1][0] - Image[x2][y2][0]) + abs_int128(Image[x1][y1][1] - Image[x2][y2][1]) + abs_int128(Image[x1][y1][2] - Image[x2][y2][2]));
                    fEdgeDis[idx1][idx2] = tmp;
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
}

vector<int> string_to_vector_int(string str, const char delim = ' ') {
    vector<int> vec;

    replace(str.begin(), str.end(), delim, ' ');
    istringstream iss(str);

    string str_buf;
    while (iss >> str_buf) {
        if (str_buf == "")
            continue;
        vec.emplace_back(stoi(str_buf)-1);
    }

    return vec;
}

vector<vector<int>> readPop(const string path) {
	cout << "read initial pop from file " << path << endl;
	vector<vector<int>> pop;
	
	ifstream ifs(path);
	string str_buf;
    if (ifs.fail()) {
        cerr << "failed to open population file " << path << endl;
        exit(-1);
    }
	int cntPop = 0;
    while (getline(ifs, str_buf)) {
		// cout << "line " << cntPop << endl;
        if (str_buf == "")
            break;
        getline(ifs, str_buf);
        vector<int> route = string_to_vector_int(str_buf, ' ');	
		pop.push_back(route);
		cntPop++;
		// if (cntPop >= 2)
		// 	break;
	}
	return pop;
}