/******************************************************************************\
*								 Class: candidates (used by gpx)			   *
\******************************************************************************/
#include "defs.h"
#include <cmath>
#include <cstdlib>
#include "Grafo.h"							// Graph class
#include "BuscaEmProfundidade.h"			// Deep First Search  class		(it is used inn compGraph) 	

using cap7_listaadj_autoreferencia::Grafo; 

class candidates {
private:	
	typedef struct {
		int num;
		int time;
	} gate_structure;
	typedef struct {
		gate_structure *inputs;
  	    gate_structure *outputs;
  	    gate_structure first_entry;
  	    gate_structure last_exit;
		double fitness;
    } tour;
	int *id;			// vector with the id(candidate component) of each node
	int *size;			// vector
	int *n_inputs;
	int *n_outputs;
	tour *blue, *red;
	int n;			// size of the tours	
    int **M_neigh;  // neighbourhood matrix: first collumn indicates the number of neighbours, and the collumns 2 and 3 indicate the index of the neighbours
	int **M_neigh2;  // neighbourhood matrix: the collumns indicate the number of conextions to the neighbours indicated in collumns 2 and 3
	int *test;		// test of the candidtes: 1 - true component; 0 - otherwise
	int isequal(Grafo *G1 , Grafo *G2);			// test if two graphs are equal
	void testSol(int *sol, int n_sol);		// test solutions
public:
	int n_cand;				// number of candidates
    candidates(int *vector_comp, int n_new);
    ~candidates(void);
	void findInputs(int *sol_blue, int *sol_red);
	void testComp(int cand);
	int testUnfeasibleComp(int *sol_blue);	
	void fusion(int *sol_blue, int *sol_red);
	void fusionB(int *sol_blue, int *sol_red);
	double off_gen(int *sol_blue, int *sol_red, int *offspring, int *label_list);
	void print(void);
	int print_test(int cand);
	int print_ninputs(int cand);
};


candidates::candidates(int *vector_comp, int n_new ){
	int i, j;

	n = n_new;
	n_cand = 0;		// number of candidate components
    for (i=0;i<n;i++)
		if (vector_comp[i]>n_cand)
			n_cand=vector_comp[i];		// remember that the first component has label 0
	n_cand++;
	size=new int[n_cand];				// size of each component
	id = new int[n];
	n_inputs = new int[n_cand];
	n_outputs = new int[n_cand];
	M_neigh=aloc_matrixi (n_cand,3);
	M_neigh2=aloc_matrixi (n_cand,2);
	for (i=0;i<n_cand;i++){
		size[i]=0;		
	}
	for (i=0;i<n;i++){
		id[i]=vector_comp[i];
		j= id[i];
		size[j]=size[j]+1;
	}	
	test = new int[n_cand];
	blue = new tour[n_cand];
	red= new tour[n_cand];
}


candidates::~candidates(void){
	int i;
	delete [] size;
	delete [] id;
	delete [] n_inputs;
	delete [] n_outputs;
	desaloc_matrixi (M_neigh,n_cand);
	desaloc_matrixi (M_neigh2,n_cand);
	delete [] test;
	for (i=0;i<n_cand;i++){
		delete [] blue[i].inputs;
		delete [] blue[i].outputs;
		delete [] red[i].inputs;
		delete [] red[i].outputs;
	}
	delete [] blue;
	delete [] red;                  
}  


// test solutions
void candidates::testSol(int *sol, int n_sol){
	int *testsol, i;
	
	testsol=aloc_vectori(n_sol);
	
	// testing blue solution
	for (i=0;i<n_sol;i++)
		testsol[i]=0;
	for (i=0;i<n_sol;i++){
		if (sol[i]<0 || sol[i]>=n_sol){
			cout<<"PROBLEM IN SOLUTION! ELEMENT SOL["<<i<<"] OUT OF RANGE (="<<sol[i]<<") "<<endl;
			exit(0);
		} 
		testsol[sol[i]]=testsol[sol[i]]+1;		
	}
	for (i=0;i<n_sol;i++){
		if (testsol[i]==0){
			cout<<"PROBLEM IN SOLUTION! ELEMENT "<<i<<" DOES NOT APPEAR "<<endl;
			exit(0);
		} 
		else if (testsol[i]>1){
			cout<<"PROBLEM IN SOLUTION! ELEMENT "<<i<<" APPEARS MORE THAN ONE TIME "<<endl;
			exit(0);
		}
	}
	
	delete [] testsol;
	
}


// find the inputs of the candidate components
void candidates::findInputs(int *sol_blue, int *sol_red){
	int i, j, k, aux, aux2, i_l, i_h, comp_size, n_reduc, *sol_blue_reduc, *sol_red_reduc, *sol_blue_reduc_t, *sol_red_reduc_t;
	gate_structure gate;

	// test solutions
	//testSol(sol_blue, n);
	//testSol(sol_red, n);

	for (i=0;i<n_cand;i++){	
		comp_size = ceil(size[i]/2);	   // the inputs/outputs are created with maximum size
		blue[i].inputs = new gate_structure[comp_size];
		blue[i].outputs = new gate_structure[comp_size];
		blue[i].first_entry.num=-1;
		red[i].inputs = new gate_structure[comp_size];
		red[i].outputs = new gate_structure[comp_size];
		red[i].first_entry.num=-1;
	}

	// Solutions without common edges
	sol_blue_reduc=aloc_vectori (n);
	sol_red_reduc=aloc_vectori (n);
	sol_blue_reduc_t=aloc_vectori (n);
	sol_red_reduc_t=aloc_vectori (n);
	j=0;
	k=0;
	for (i=0;i<n;i++){
		aux=sol_blue[i];
		if (size[id[aux]]>1){
			sol_blue_reduc[j]=aux;	
			sol_blue_reduc_t[j]=i;	
			j++;
		}
		aux=sol_red[i];
		if (size[id[aux]]>1){
			sol_red_reduc[k]=aux;
			sol_red_reduc_t[k]=i;	
			k++;
		}
	}
	n_reduc=j;
		
	// Blue Tour
	for (i=0;i<n_cand;i++){
		n_inputs[i]=0;
		n_outputs[i]=0;		
		M_neigh[i][0]=0;		// the first column indicates the number of neighbours		
	}
	for (i=0;i<n_reduc;i++){
			gate.num=sol_blue_reduc[i];
			gate.time=sol_blue_reduc_t[i];
			if (i==0)
                i_l=n_reduc-1;
            else
                i_l=i-1;
			if (i==(n_reduc-1))
                i_h=0;
            else
                i_h=i+1;
            aux=id[sol_blue_reduc[i]];
            aux2=id[sol_blue_reduc[i_l]];
			if ( aux != aux2  ){				
                blue[ aux ].inputs[ n_inputs[aux] ] = gate;
				n_inputs [ aux ] = n_inputs[ aux ] + 1;
				if ( blue[ aux ].first_entry.num == -1 )
					blue[ aux ].first_entry = gate;		// records the first entry in the candidate for the blue tour
				// Updating the neighbourhood relations
				if (M_neigh [ aux ] [ 0 ] ==0)	{				
					M_neigh [ aux ] [ 0 ] = 1;
					M_neigh [ aux ] [ 1 ] = aux2;
					M_neigh2 [ aux ] [ 0 ] = 1;
				}
				else if (M_neigh [ aux ] [ 0 ] ==1)	{
					if ( M_neigh [ aux ] [ 1 ] == aux2 )
						M_neigh2 [ aux ] [ 0 ] = M_neigh2 [ aux ] [ 0 ] + 1;
					else {
						M_neigh [ aux ] [ 0 ] = 2;
						M_neigh [ aux ] [ 2 ] = aux2;
						M_neigh2 [ aux ] [ 1 ] = 1;
					}
				}
				else{
					if ( M_neigh [ aux ] [ 1 ] == aux2 )
						M_neigh2 [ aux ] [ 0 ] = M_neigh2 [ aux ] [ 0 ] + 1;
					else if ( M_neigh [ aux ] [ 2 ] == aux2 )
						M_neigh2 [ aux ] [ 1 ] = M_neigh2 [ aux ] [ 1 ] + 1;
					else
						M_neigh [ aux ] [ 0 ] = M_neigh [ aux ] [ 0 ] +1;
				}							 
			}
			aux2=id[sol_blue_reduc[i_h]];	
			if ( aux != aux2 ){
                blue[ aux ].outputs[ n_outputs[aux] ] = gate;
				n_outputs[ aux ] = n_outputs[ aux ] + 1;					
				blue[ aux ].last_exit = gate;		// records the last exit in the candidate for the blue tour
				// Updating the neighbourhood relations
				if (M_neigh [ aux ] [ 0 ] ==0)	{				
					M_neigh [ aux ] [ 0 ] = 1;
					M_neigh [ aux ] [ 1 ] = aux2;
					M_neigh2 [ aux ] [ 0 ] = 1;
				}
				else if (M_neigh [ aux ] [ 0 ] ==1)	{
					if ( M_neigh [ aux ] [ 1 ] == aux2 )
						M_neigh2 [ aux ] [ 0 ] = M_neigh2 [ aux ] [ 0 ] + 1;
					else {
						M_neigh [ aux ] [ 0 ] = 2;
						M_neigh [ aux ] [ 2 ] = aux2;
						M_neigh2 [ aux ] [ 1 ] = 1;
					}
				}
				else{
					if ( M_neigh [ aux ] [ 1 ] == aux2 )
						M_neigh2 [ aux ] [ 0 ] = M_neigh2 [ aux ] [ 0 ] + 1;
					else if ( M_neigh [ aux ] [ 2 ] == aux2 )
						M_neigh2 [ aux ] [ 1 ] = M_neigh2 [ aux ] [ 1 ] + 1;
					else
						M_neigh [ aux ] [ 0 ] = M_neigh [ aux ] [ 0 ] +1;
				}							 
			}		      
	}
	// Red Tour
	for (i=0;i<n_cand;i++){
		n_inputs[i]=0;
		n_outputs[i]=0;		
	}
	for (i=0;i<n_reduc;i++){
			gate.num=sol_red_reduc[i];
			gate.time=sol_red_reduc_t[i];
			if (i==0)
                i_l=n_reduc-1;
            else
                i_l=i-1;
			if (i==(n_reduc-1))
                i_h=0;
            else
                i_h=i+1;
            aux=id[sol_red_reduc[i]];
			if ( aux != id[sol_red_reduc[i_l]] ){
                red[ aux ].inputs[ n_inputs[aux] ] = gate;
				n_inputs [ aux ] = n_inputs[ aux ] + 1;	
				if ( red[ aux ].first_entry.num == -1 )
					red[ aux ].first_entry = gate;		// records the first entry in the candidate for the red tour						 
			}
			if ( aux != id[sol_red_reduc[i_h]] ){
                red[ aux ].outputs[ n_outputs[aux] ]=gate;
				n_outputs[ aux ] = n_outputs[aux]+1;		
				red[ aux ].last_exit = gate;		// records the last exit in the candidate for the red tour	
			}		      
	}
		
	delete [] sol_blue_reduc;
	delete [] sol_red_reduc;
	delete [] sol_blue_reduc_t;
	delete [] sol_red_reduc_t;

}


// test if graphs are equal (obs.: remember that each vertex has 0 or 1 edge)
int candidates::isequal(Grafo *G1 , Grafo *G2){
	int i=0, G1_empty, G2_empty, equal=1;

	while (equal==1 && i < G1->_numVertices () ){
		G1_empty=G1->listaAdjVazia (i);	 // check if node i has an emply edge list
		G2_empty=G2->listaAdjVazia (i);	// check if node i has an emply edge list
		if (G1_empty != G2_empty)
			equal=0;
		else{
			if(G1_empty==0){
				Grafo::Aresta *a = G1->primeiroListaAdj(i);	 // first edge of the adjacency list for node i
				Grafo::Aresta *b = G2->primeiroListaAdj(i);	// first edge of the adjacency list for node i
				if (a->_v2 () != b->_v2 () ){
					equal=0;
				}
				delete a;
				delete b;
			}
		}				
		i++;
    }

	return equal;
}
    

// test candidate component cand
void candidates::testComp(int cand){
	int i, *inp_out_blue_inv, *inp_out_red;

	if (size[cand]<=1)
		test[cand] = -1;
	else {
		if (n_inputs[cand]<1)
			test[cand] = 0;
		else {
		    if ( n_inputs[cand]==1)
		        test[cand]=1;   
			else {       

		        // Graphs for blue and red tours			
				inp_out_blue_inv = new int[n];
				inp_out_red = new int[2*n_inputs[cand]];			  
		        // Two cases: Case 1 - first input before first output; Case 2 - otherwise
				// Obs.: remember that the inputs and outputs are inserted according to the flow				
				if (blue[cand].inputs[0].time < blue[cand].outputs[0].time ){
				    for (i=0;i<n_inputs[cand];i++){																
				        inp_out_blue_inv[ blue[cand].inputs[i].num ]=2*i;
				        inp_out_blue_inv[ blue[cand].outputs[i].num ]=2*i+1; 				        	
					} 
				}
				else{
					for (i=0;i<n_inputs[cand];i++){									
				        inp_out_blue_inv[ blue[cand].outputs[i].num ]=2*i;
				        inp_out_blue_inv[ blue[cand].inputs[i].num ]=2*i+1; 			
					} 
				}
		
				if (red[cand].inputs[0].time < red[cand].outputs[0].time ){
				    for (i=0;i<n_inputs[cand];i++){									     
						inp_out_red[2*i]=red[cand].inputs[i].num;				       
						inp_out_red[2*i+1]=red[cand].outputs[i].num;								
					} 
				}
				else{
					for (i=0;i<n_inputs[cand];i++){					
						inp_out_red[2*i]=red[cand].outputs[i].num;				       
						inp_out_red[2*i+1]=red[cand].inputs[i].num;								
					} 
				}	
				Grafo *Gs_blue = new Grafo (2*n_inputs[cand]);			// simplified graph for the blue path inside the component cand
				Grafo *Gs_red = new Grafo (2*n_inputs[cand]);			// simplified graph for the red path inside the component cand
				// Two cases: Case 1 - first input before first output; Case 2 - otherwise
				// Obs.: remember that the inputs and outputs are inserted according to the flow
				if (blue[cand].inputs[0].time < blue[cand].outputs[0].time ){
					for (i=0;i<(2*n_inputs[cand]-1);i=i+2){
						Gs_blue->insereAresta (i, i+1, 1);	  // insert edge
						Gs_blue->insereAresta (i+1, i, 1);	  // insert edge
					}
				}
				else {
					for (i=1;i<(2*n_inputs[cand]-2);i=i+2){
						Gs_blue->insereAresta (i, i+1, 1);	  // insert edge	
						Gs_blue->insereAresta (i+1, i, 1);	  // insert edge
					}
					Gs_blue->insereAresta (2*n_inputs[cand]-1, 0, 1);	  // insert edge			
					Gs_blue->insereAresta (0, 2*n_inputs[cand]-1, 1);	  // insert edge	
				}
					
				if (red[cand].inputs[0].time < red[cand].outputs[0].time ){
					for (i=0;i<(2*n_inputs[cand]-1);i=i+2){
						Gs_red->insereAresta ( inp_out_blue_inv[ inp_out_red[i] ], inp_out_blue_inv[ inp_out_red[i+1] ], 1);	  // insert edge
						Gs_red->insereAresta ( inp_out_blue_inv[ inp_out_red[i+1] ],inp_out_blue_inv[ inp_out_red[i] ], 1);	  // insert edge
					}
				}
				else {
					for (i=1;i<(2*n_inputs[cand]-2);i=i+2){
						Gs_red->insereAresta ( inp_out_blue_inv[ inp_out_red[i] ], inp_out_blue_inv[ inp_out_red[i+1] ], 1);	  // insert edge	
						Gs_red->insereAresta ( inp_out_blue_inv[ inp_out_red[i+1] ],inp_out_blue_inv[ inp_out_red[i] ], 1);	  // insert edge	
					}
					Gs_red->insereAresta ( inp_out_blue_inv[ inp_out_red[2*n_inputs[cand]-1] ], inp_out_blue_inv[ inp_out_red[0] ], 1);	  // insert edge		
					Gs_red->insereAresta ( inp_out_blue_inv[ inp_out_red[0] ], inp_out_blue_inv[ inp_out_red[2*n_inputs[cand]-1] ],  1);	  // insert edge	
				}
		
				// Comparing the two graphs					
				if ( isequal(Gs_blue,Gs_red) ) 
		            test[cand]=1;
		        else
		            test[cand]=0;
      	
				delete Gs_red;
				delete Gs_blue;
				delete [] inp_out_blue_inv;
				delete [] inp_out_red;
			}
		}
	}
}


// test if simplified graphs outside unfesible candidate component are equal
// Observation: this is equivalent of testing if all entries for a component are grouped
// after removing the feasible components (identified according to testComp) of the 
// list of candidate entries
int candidates::testUnfeasibleComp(int *sol_blue){
	int i, j, aux, *comp_seq, *inp_comp_seq, n_newpart=0;
	
	comp_seq=aloc_vectori(n);			// sequence of all entries in unfeasible components in the order given by sol_blue
	inp_comp_seq=aloc_vectori(n_cand);		// records the number of entries in each component in comp_seq
	
	// creating comp_seq
	j=0;									// j is the effective size of comp_seq
	aux= id[sol_blue[0]];
	if ( test[ aux ]==0 && n_inputs[aux]>0 && size[aux]>1 && aux != id[sol_blue[n-1]] ){
		comp_seq[j]=aux;
		j++;
	}
	inp_comp_seq[aux]=0;	
	for (i=1;i<n;i++){
		aux=id[sol_blue[i]];
		if (test[ aux ]==0 && n_inputs[aux]>0 && size[aux]>1 && aux != id[sol_blue[i-1]]){
			comp_seq[j]=aux;
			j++;
		}
		inp_comp_seq[aux]=0;					
	}
	
	// testing by checking the grouping of  the components (i.e., testing if the number of entries is 2)
	if (j>0){	
		aux=comp_seq[0];	
		if (aux != comp_seq[j-1])
				inp_comp_seq[ aux ]=inp_comp_seq[ aux ] +1;
		for (i=1;i<j;i++){
			aux=comp_seq[i];
			if (aux != comp_seq[i-1])
				inp_comp_seq[ aux ]=inp_comp_seq[ aux ] +1;
		}
		for (i=0;i<n_cand;i++){
			if ( test[i]==0 &&  inp_comp_seq[i]==1 ){
				test[i]=1;
				n_newpart++;
			}
											
		}
		
	}
	
	delete [] inp_comp_seq;
	delete [] comp_seq;
	
	return (n_newpart);
}



// if candidate cand did not pass the test and has conditions, apply fusion with the neighbour with more connections (for more than 2 cutting points)
void candidates::fusion(int *sol_blue, int *sol_red){
	int i, cand, aux, n_fusions=0, *neigh_vec_cond, *neigh_vec_ind;
	
	neigh_vec_cond = new int[n_cand];   // cond= -1 if cand is a true component or if it is between two common edges ; =0 stil does not chosen ; =1 already chosen, and id will be changed; =2 already chosen but id will not be changed 
	neigh_vec_ind = new int[n_cand];		// neigh_vec_ind: indicates the neighbour that will be fusioned with cand

	for (cand=0;cand<n_cand;cand++){
		if (test[cand]==1 || size[cand]<=1)
			neigh_vec_cond[cand]=-1;		
		else
			neigh_vec_cond[cand]=0;
	}
	
	for (cand=0;cand<n_cand;cand++){   		
		if (neigh_vec_cond[cand]==0){			
			if (M_neigh[cand][0]==1){
				aux=M_neigh[cand][1];
				if (neigh_vec_cond[ aux ]==0){
					neigh_vec_ind[cand]=aux ;
					neigh_vec_cond[ cand ]=1;
					neigh_vec_cond [ aux ]=2;
					n_fusions++;
				}
			}
			else if (M_neigh[cand][0]==2){
				if ( M_neigh2[cand][0]  > M_neigh2[cand][1]   ){
					aux=M_neigh[cand][1];
					if ( neigh_vec_cond[ aux ]==0 ){
						neigh_vec_ind[cand]=aux ;
						neigh_vec_cond[ cand ]=1;
						neigh_vec_cond [ aux ]=2;
						n_fusions++;
					}
				}
				else {
					aux=M_neigh[cand][2];
					if ( neigh_vec_cond[ aux ]==0 ){
						neigh_vec_ind[cand]=aux ;
						neigh_vec_cond[ cand ]=1;
						neigh_vec_cond [ aux ]=2;
						n_fusions++;
					}
				}
			}	
			
		}
	}

	if (n_fusions>0){
		
		// Reseting tour structures blue and red 
		for (cand=0;cand<n_cand;cand++){
			delete [] blue[cand].inputs;
			delete [] blue[cand].outputs;
			delete [] red[cand].inputs;
			delete [] red[cand].outputs;
		}
		delete [] blue;
		delete [] red;
		blue = new tour[n_cand];
		red= new tour[n_cand];			

		// Making the fusions
		for (i=0;i<n;i++){
			aux=id[i];
			if ( neigh_vec_cond[ aux ]==1 ){
				size[ aux ] = size[ aux ]  - 1;
				id[i] = neigh_vec_ind[ aux ];
				aux=id[i];
				size[ aux ] = size[ aux ]  + 1;
			}
		}
			
		// Repeating Step 5: Finding the inputs and outputs of each candidate component
		findInputs(sol_blue,sol_red);
		// Repeating Step 6: testing the candidate components
		for (cand=0;cand<n_cand;cand++)
			if (neigh_vec_cond[ cand ]==2)
				testComp(cand); // test component cand  		
	
	}	
		
	delete [] neigh_vec_cond;
	delete [] neigh_vec_ind;

}


// fusions of the candidate components in order to create partitions with two cutting points
void candidates::fusionB(int *sol_blue, int *sol_red){
	int i, cand, n_cand_seq=0, previous_cand, next_cand, n_cand_new, n_rounds=0, n_rounds_max=1000;
	int *cand_seq, *cand_seq_cut, *assigned_cand, *new_label, *vector_new_cand, *new_component, n_newpart;	
	//int  time_window_blue, time_window_red;
	
	
	// Memory allocation
	cand_seq = aloc_vectori(n);			// list of entries and exits of unfeasible candidates
	cand_seq_cut = aloc_vectori(n);
	new_label = aloc_vectori(n_cand);
	new_component= aloc_vectori(n_cand);
	Grafo *G_cand = new Grafo (n_cand);			// create object graph (from grafo.h)
		

	// Walking in the blue tour and finding the high level cuts
	previous_cand=id[sol_blue[n-1]];				
	for (i=0;i<n;i++){
		cand=id[ sol_blue[i] ];	// candidate for vertex i of the blue tour
		if (i==(n-1))
			next_cand=id[sol_blue[0]];
		else
			next_cand=id[sol_blue[i+1]];
		// test if it is an unfeasible partition
		if (test[cand]==0 && n_inputs[cand]>0 && size[cand]>1){
			if ( cand != previous_cand || cand != next_cand ){
				cand_seq[n_cand_seq]=cand;
				// checking if it is a first common entry or last common exit
				if ( (sol_blue[i]==blue[cand].first_entry.num) && ( (blue[cand].first_entry.num==red[cand].first_entry.num) || (blue[cand].first_entry.num==red[cand].last_exit.num) ) )
					cand_seq_cut[n_cand_seq]=1;			// it is a first common entry		
				else if ( (sol_blue[i]==blue[cand].last_exit.num) && ( (blue[cand].last_exit.num==red[cand].last_exit.num) ||  (blue[cand].last_exit.num==red[cand].first_entry.num) ) )
					cand_seq_cut[n_cand_seq]=1;			// it is a last common exit	
				else
					cand_seq_cut[n_cand_seq]=0;								
				n_cand_seq++;
			}			
		}
		previous_cand= cand;
	}
	// building the graph with conections between unfeasible components, but without main entries and main exits
	if (n_cand_seq>0){
		for (i=0;i<n_cand_seq-1;i++){
			if (cand_seq[i] != cand_seq[i+1] && cand_seq_cut[i]==0 && cand_seq_cut[i+1]==0){
				G_cand->insereAresta (cand_seq[i], cand_seq[i+1], 1); 			// insert edge between candidates
		  		G_cand->insereAresta (cand_seq[i+1], cand_seq[i], 1);			// insert edge between candidates				
			}			
		}
		if (cand_seq[n_cand_seq-1] != cand_seq[0] && cand_seq_cut[n_cand_seq-1]==0 && cand_seq_cut[0]==0){
			G_cand->insereAresta (cand_seq[n_cand_seq-1], cand_seq[0], 1); 			// insert edge between candidates
		  	G_cand->insereAresta (cand_seq[0], cand_seq[n_cand_seq-1], 1);			// insert edge between candidates				
		}
	}
	
	// Walking in the red tour and finding the high level cuts
	n_cand_seq=0;
	previous_cand=id[sol_red[n-1]];				
	for (i=0;i<n;i++){
		cand=id[ sol_red[i] ];	// candidate for vertex i of the red tour
		if (i==(n-1))
			next_cand=id[sol_red[0]];
		else
			next_cand=id[sol_red[i+1]];
		// test if it is an unfeasible partition
		if (test[cand]==0  && n_inputs[cand]>0 && size[cand]>1){
			if ( cand != previous_cand || cand != next_cand ){
				cand_seq[n_cand_seq]=cand;
				// checking if it is a first common entry or last common exit
				if ( (sol_red[i]==red[cand].first_entry.num) && ( (red[cand].first_entry.num==blue[cand].first_entry.num) || (red[cand].first_entry.num==blue[cand].last_exit.num) ) )
					cand_seq_cut[n_cand_seq]=1;			// it is a first common entry		
				else if ( (sol_red[i]==red[cand].last_exit.num) && ( (red[cand].last_exit.num==blue[cand].last_exit.num) ||  (red[cand].last_exit.num==blue[cand].first_entry.num) ) )
					cand_seq_cut[n_cand_seq]=1;			// it is a last common exit	
				else
					cand_seq_cut[n_cand_seq]=0;								
				n_cand_seq++;
			}			
		}
		previous_cand= cand;
	}
	// building the graph with connections between unfeasible components, but without main entries and main exits
	if (n_cand_seq>0){
		for (i=0;i<n_cand_seq-1;i++){
			if (cand_seq[i] != cand_seq[i+1] && cand_seq_cut[i]==0 && cand_seq_cut[i+1]==0){
				G_cand->insereAresta (cand_seq[i], cand_seq[i+1], 1); 			// insert edge between candidates
		  		G_cand->insereAresta (cand_seq[i+1], cand_seq[i], 1);			// insert edge between candidates				
			}			
		}
		if (cand_seq[n_cand_seq-1] != cand_seq[0] && cand_seq_cut[n_cand_seq-1]==0 && cand_seq_cut[0]==0){
			G_cand->insereAresta (cand_seq[n_cand_seq-1], cand_seq[0], 1); 			// insert edge between candidates
		  	G_cand->insereAresta (cand_seq[0], cand_seq[n_cand_seq-1], 1);			// insert edge between candidates				
		}
	}
			
	for (cand=0;cand<n_cand;cand++){			
		new_label[cand]=cand;
	}
	
	if (n_cand_seq>0){
	
		vector_new_cand=aloc_vectori(n_cand);					// fusion of candidates	
		
		cap7::BuscaEmProfundidade cfc(G_cand);					// object connected componets (from BuscaEmProfundidade.h)
	    cfc.compCon (vector_new_cand);							// find the connected components for the graph
		cfc.~BuscaEmProfundidade ();
							
		// new label
		n_cand_new=-1;
		for (i=0;i<n_cand;i++){
			new_component[ i ]=0;
			if (n_cand_new<vector_new_cand[i]) 
				n_cand_new=vector_new_cand[i];
		}
		
		if (n_cand_new>-1){
			n_cand_new++;
			assigned_cand=aloc_vectori(n_cand_new);
		
			for (cand=0;cand<n_cand_new;cand++){			
				assigned_cand[cand]=-1;
			}
			for (cand=0;cand<n_cand;cand++){
				if (test[cand]==0  && n_inputs[cand]>0 && size[cand]>1 ){					
					if ( assigned_cand[ vector_new_cand[cand] ] == -1){					
						assigned_cand[vector_new_cand[cand] ]=cand;		
						new_component[ cand ]=1;
					}
					new_label[ cand ]=assigned_cand[ vector_new_cand[cand] ];
				}
			}
			delete [] assigned_cand;
					
			// Reseting tour structures blue and red 
			for (cand=0;cand<n_cand;cand++){
				delete [] blue[cand].inputs;
				delete [] blue[cand].outputs;
				delete [] red[cand].inputs;
				delete [] red[cand].outputs;
			}
			delete [] blue;
			delete [] red;
			blue = new tour[n_cand];
			red= new tour[n_cand];			
					
			// Making the fusions
			for (i=0;i<n;i++){		
				cand=id[i];
				if ( new_label[ cand ] != cand ){
					size[ cand ] = size[ cand ]  - 1;
					id[i] = new_label[ cand ];
					cand=id[i];
					size[ cand ] = size[ cand ]  + 1;
				}
			}
						
			// Repeating Step 5: Finding the inputs and outputs of each candidate component
			findInputs(sol_blue,sol_red);
	
			// Repeating Step 6: testing the candidate components
			// this procedure is O(n)
			for (cand=0;cand<n_cand;cand++)
				if (new_component[ cand ]==1)
					testComp(cand); // test component cand  		
	
			
			// Testing unfeasible partitions 
			do {
				n_rounds++;
				n_newpart=testUnfeasibleComp(sol_blue);
			} while (n_newpart>0 && n_rounds<=n_rounds_max);					
		}
						
		delete [] vector_new_cand;

	}
	delete G_cand;	
	delete [] cand_seq;
	delete [] cand_seq_cut;
	delete [] new_label;
	delete [] new_component;

}



// select between the blue and red paths for each component
double candidates::off_gen(int *sol_blue, int *sol_red, int *offspring, int *label_list){
	int i,i_l,i_h,k, aux, aux2, *select_cand, select_rest, *offspring_p2, *sol_blue_index, *sol_red_index, v_aux;
	double blue_fitness_rest, red_fitness_rest, fitness;

	// Computing the Fitness of each subtour in each true component
	for (i=0;i<n_cand;i++){ 
		blue[i].fitness=0;
		red[i].fitness=0;
	}
	blue_fitness_rest=0;		//  fitness of the parts that are not true components in the blue path
	red_fitness_rest=0;			//  fitness of the parts that are not true components in the red path
	for (i=0;i<n;i++){
		if (i<n-1)
			i_h=i+1;
		else
			i_h=0;
		aux=sol_blue[i];
		aux2=sol_blue[i_h];
		if ( (test[id[aux]]>0) && (id[aux]==id[aux2]) ){
			if (label_list[aux]!=label_list[aux2])			
				blue[id[aux]].fitness=blue[id[aux]].fitness+weight(label_list[aux],label_list[aux2]);
		}
		else{
			if (label_list[aux]!=label_list[aux2])
				blue_fitness_rest=blue_fitness_rest+weight(label_list[aux],label_list[aux2]);
		}
	    aux=sol_red[i];
		aux2=sol_red[i_h];
		if ( (test[id[aux]]>0) && (id[aux]==id[aux2]) ){
			if (label_list[aux]!=label_list[aux2])
				red[id[aux]].fitness=red[id[aux]].fitness+weight(label_list[aux],label_list[aux2]);
		}
		else
			if (label_list[aux]!=label_list[aux2])
				red_fitness_rest=red_fitness_rest+weight(label_list[aux],label_list[aux2]);
	}


	// Selecting the components
	if (blue_fitness_rest>red_fitness_rest)
		select_rest=1;		// 1 for red and 0 for blue
	else
		select_rest=0;
	
	select_cand=new int[n];
	
	for (i=0;i<n_cand;i++){
		if (test[i]<1){
			select_cand[i]=select_rest;
		}
		else{
			if (blue[i].fitness>red[i].fitness)
				select_cand[i]=1;
			else
				select_cand[i]=0;
		}
	}

	// Generating the offspring	
	// GPX2
	// index for the solutions
	sol_blue_index=new int[n];
	sol_red_index=new int[n];
	for (i=0;i<n;i++){
		sol_blue_index[sol_blue[i]]=i;
		sol_red_index[sol_red[i]]=i;	
	}
	// Offspring: Graph

	Grafo *G_offspring = new Grafo (n);			// graph for offspring
	for (i=0;i<n;i++){
		if (select_cand[id[i]]==0){
			if (sol_blue_index[i]==0){
				i_l=sol_blue[n-1];
				i_h=sol_blue[sol_blue_index[i]+1];
			}
			else if (sol_blue_index[i]==(n-1)){
				i_l=sol_blue[sol_blue_index[i]-1];
				i_h=sol_blue[0];
			}	
			else {
				i_l=sol_blue[sol_blue_index[i]-1];
				i_h=sol_blue[sol_blue_index[i]+1];
			}
		}
		else{
			if (sol_red_index[i]==0){
				i_l=sol_red[n-1];
				i_h=sol_red[sol_red_index[i]+1];
			}
			else if (sol_red_index[i]==(n-1)){
				i_l=sol_red[sol_red_index[i]-1];
				i_h=sol_red[0];
			}	
			else {
				i_l=sol_red[sol_red_index[i]-1];
				i_h=sol_red[sol_red_index[i]+1];
			}
		}
		G_offspring->insereAresta (i,i_l,1);	   // insert edge
		G_offspring->insereAresta (i,i_h,1);   // insert edge 
		
	}
	
	// Offspring: vector
	offspring_p2=new int[n];
	i=0;
	offspring_p2[i]=0;
	v_aux=0;
	Grafo::Aresta *a = G_offspring->primeiroListaAdj(v_aux);	// first edge of the vertex	
	Grafo::Aresta *b = G_offspring->proxAdj(v_aux);	  			// next edge of the vertex	
	v_aux=a->_v2 ();
	i++;
	offspring_p2[i]=v_aux;
	delete a;

	while(v_aux>0){
		a = G_offspring->primeiroListaAdj(v_aux); 	// first edge of the vertex
		b = G_offspring->proxAdj (v_aux);		 	// next edge of the vertex	
		if (a->_v2 () == offspring_p2[i-1] ){
			v_aux=b->_v2 ();	
		}
		else{
			v_aux=a->_v2 ();	
		}
		delete a;
		delete b;

		i++;
		if (v_aux>0){
			offspring_p2[i]=v_aux;
		}
	}
	
	delete G_offspring;

	// Fitness of the offspring
	fitness=0;
	for (i=0;i<n_cand;i++){
		if (test[i]==1){
			if (select_cand[i] == 0 )
				fitness=fitness+blue[i].fitness;
			else
				fitness=fitness+red[i].fitness;		
		}
	}
	if (select_rest==0)
		fitness=fitness+blue_fitness_rest;
	else
		fitness=fitness+red_fitness_rest;

	// removing the ghost nodes
	k=0;
	for (i=0;i<n;i++){
		if (offspring_p2[i]<n_cities){
			offspring[k]=offspring_p2[i];		
			k++;
		}	
	}
	//testSol(offspring, n_cities);
	
	delete [] sol_red_index;
	delete [] sol_blue_index; 	
	delete [] select_cand;
	delete [] offspring_p2;

	return fitness;
}


// Print the candidates
void candidates::print(void){
	int i;
	
	for (i=0;i<n_cand;i++)
		if (size[i]>1 && !test[i]){	
			cout<< "Candidate ("<<i<<"), Size: "<<size[i]<<", Number of inputs: "<< n_inputs[i]<<endl;
			cout<< "  blue tour: First Entry: "<< blue[i].first_entry.num<<" , "<< blue[i].first_entry.time <<"  Last Exit: "<< blue[i].last_exit.num<<" , "<< blue[i].last_exit.time <<endl;
			cout<< "  red tour: First Entry: "<< red[i].first_entry.num<<" , "<< red[i].first_entry.time <<"  Last Exit: "<< red[i].last_exit.num<<" , "<< red[i].last_exit.time <<endl;		
		}
/*	cout<< "Components:"<< endl;
	for (i=0;i<n;i++)
		cout<<id[i]<<", ";
	cout<<endl;*/
		
	
}

// return test of the candidate
int candidates::print_test(int cand){
		return (test[cand]);
}

// return number of inputs of the candidate
int candidates::print_ninputs(int cand){
		return (n_inputs[cand]);
}
