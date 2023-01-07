/******************************************************************************\
*								 Definitions							 *
\******************************************************************************/
#include <iostream>
#include <string>
#include <vector>
#include "int128.h"
using namespace std; 
using lint = long long;
using int128 = __int128;

/* Global variables */
extern double *coord_x, *coord_y;				// coordinates of the cities
// extern int **W;									// weight matrix for the ATSP
extern int128** fEdgeDis;  /* EdgeDis[i][j]: distance between i and j */
// extern vector<vector<int128>> W;
extern int n_cities;							// number of cities
extern char *prob_name;							// name of the file for the weight matrix

extern int128 INF;
extern int magnification_digits;
extern int128 Magnification;

/* Function declaration */
int128 gpx(vector<int>& solution_blue, vector<int>& solution_red, vector<int>& offspring );
int *aloc_vectori(int lines);
double *aloc_vectord(int lines);
int **aloc_matrixi(int lines , int collums);
void desaloc_matrixi(int **Matrix , int lines);
void read_problem(char* filename, string init_tour_name);
void rand_perm(int *inp, int *out, int size);
// int weight(int i, int j);

int128 weight(int i, int j);
int encode(int x, int y);
pair<int, int> decode(int i);
int encodesmall(int dx, int dy);
pair<int, int> decodesmall(int i);
bool checkin(int x, int y);
vector<vector<int>> readPop(const string path);
