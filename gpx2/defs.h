/******************************************************************************\
*								 Definitions							 *
\******************************************************************************/
#include <iostream>
using namespace std; 

/* Global variables */
extern double *coord_x, *coord_y;				// coordinates of the cities
extern int **W;									// weight matrix for the ATSP
extern int n_cities;							// number of cities
extern char *prob_name;							// name of the file for the weight matrix

/* Function declaration */
double gpx(int *solution_blue, int *solution_red, int *offspring );
int *aloc_vectori(int lines);
double *aloc_vectord(int lines);
int **aloc_matrixi(int lines , int collums);
void desaloc_matrixi(int **Matrix , int lines);
void read_problem(char* filename);
void rand_perm(int *inp, int *out, int size);
int weight(int i, int j);
