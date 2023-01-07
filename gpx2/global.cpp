#include "defs.h"

// TSP
double *coord_x, *coord_y;  // coordinates of the cities
int** W;                    // weight matrix for the ATSP
int n_cities = 257 * 257;   // number of cities
char* prob_name;            // name of the file for the weight matrix
