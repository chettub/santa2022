/******************************************************************************\
*								Diverse Functions						 *
\******************************************************************************/
#include "defs.h"
#include <cstdlib>



/******************************************************************************\
*								 Dynamic Allocation: Matrix of Integers					 *
\******************************************************************************/
int **aloc_matrixi(int lines , int collums)
{
	int i, **Matrix;
	
	Matrix = new int*[lines];
	for (i=0;i<lines;i++) {
		Matrix[i] = new int[collums];
	}
	if (!Matrix) {
		cout<<"Allocation Error!"<<endl;
		exit(1);
	}

	return Matrix;
}


/******************************************************************************\
*								Dynamic Allocation: Vector of Integers						 *
\******************************************************************************/
int *aloc_vectori(int lines)
{
	int *vector;

	vector = new int[lines];
	if (!vector) {
		cout<<"Allocation Error!"<<endl;
		exit(1);
	}
	return vector;
}


/******************************************************************************\
*								Dynamic Allocation: Vector of Doubles						 *
\******************************************************************************/
double *aloc_vectord(int lines)
{
	double *vector;

	vector = new double[lines];
	if (!vector) {
		cout<<"Allocation Error!"<<endl;
		exit(1);
	}
	return vector;
}


/******************************************************************************\
*								 Dynamic Desallocation: Matrix of Integers					 *
\******************************************************************************/
void desaloc_matrixi(int **Matrix , int lines)
{
	int i;

	for(i=0;i<lines;i++) {
		delete [] Matrix[i];
	}
	delete [] Matrix;

}


/******************************************************************************\
*								Real Random Number				 *
\******************************************************************************/
double rand_number(void)
{
	return ( rand()/(RAND_MAX+1.0) );

}

/******************************************************************************\
*								 Random Permutation of a Vector of Integers    *
*								(using standard random generator)			   *
\******************************************************************************/
void rand_perm(int *inp, int *out, int size)
{
	int i, j;
	out[0]=inp[0];
	for(i=1;i<size;i++) {
		j= (int) (  rand_number() *(i-0.0)+0.0);  // random integer beteween [lim_inf=0 , lim_sup=i]
		if (i != j)
			out[i]=out[j];
		out[j]=inp[i];
	}

}


