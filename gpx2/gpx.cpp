/***************************************************************************************************\
*						 Generalized Partition Crossover 2			   							  	*
*																									*	
* Reference:  R. Tinos, D. Whitley, and G. Ochoa (2017). A new generalized partition crossover for 	*
*		the traveling salesman problem: tunneling between local optima. arXiv.org					*
*																									*
*  																									*
\***************************************************************************************************/
#include "tour.h"							// candidate components class
#include <math.h>

// compute the weights
int weight(int i, int j){
	//if (n_cities>32000)
	if (n_cities>15000)
		return round( sqrt( pow(coord_x[i]-coord_x[j],2) +pow(coord_y[i]-coord_y[j],2) ) );		// compute the distance between the cities i and j
	else
		return W[i][j];
}     


// Identifying the vertices with degree 4 and common edges
int d4_vertices_id(int *solution_blue, int *solution_red, int *d4_vertices, int *common_edges_blue, int *common_edges_red){
	int i, aux, aux2, **M_aux, n_d4_vertices; 

	//create a matrix (n_cities x 4) with all edges;
	M_aux=aloc_matrixi(n_cities,4);

	for (i=1;i<n_cities-1;i++){	
		aux=solution_blue[i];	
		M_aux[aux][0]=solution_blue[i+1];		
		M_aux[aux][1]=solution_blue[i-1];
		aux=solution_red[i];
		M_aux[aux][2]=solution_red[i+1];
		M_aux[aux][3]=solution_red[i-1];		
	}
	aux=solution_blue[0];
	M_aux[aux][0]=solution_blue[1];
	M_aux[aux][1]=solution_blue[n_cities-1];
	aux=solution_red[0];
	M_aux[aux][2]=solution_red[1];
	M_aux[aux][3]=solution_red[n_cities-1];
	aux=solution_blue[n_cities-1];
	M_aux[aux][0]=solution_blue[0];
	M_aux[aux][1]=solution_blue[n_cities-2];
	aux=solution_red[n_cities-1];
	M_aux[aux][2]=solution_red[0];
	M_aux[aux][3]=solution_red[n_cities-2];
	n_d4_vertices = 0;	  // number of degree 4 vertices

	for (i=0;i<n_cities;i++){
		d4_vertices[i]=1;				// // d4_vertices: binary vector (1: element is a degree 4 vertex; 0: otherwise);
		common_edges_blue[i]=0;
		common_edges_red[i]=0;
		aux=M_aux[i][0];
		aux2=M_aux[i][2];
		if ( (aux == aux2 ) || (aux == M_aux[i][3] ) ){
			d4_vertices[i]=0;
			common_edges_blue[i]=1;
			if (aux == aux2 )
				common_edges_red[i]=1;
		}
		aux=M_aux[i][1];
		if ( (aux == aux2 ) || (aux == M_aux[i][3] ) ){
			d4_vertices[i]=0;	
			if (aux == aux2 )
				common_edges_red[i]=1;
		}
		if (d4_vertices[i]==1)
			n_d4_vertices++;	 
	}
	
	desaloc_matrixi(M_aux,n_cities);
	
	return n_d4_vertices;
}


// Insert ghost nodes in the solution
void insert_ghost(int *solution, int *solution_p2, int *d4_vertices, int *label_list_inv){
	int i, j, aux;
	
	j=0;
	for (i=0;i<n_cities;i++){
	   aux=solution[i];
	   solution_p2[j]=aux;	
	   j++;
	   if (d4_vertices[aux]==1){
		    solution_p2[j]=label_list_inv[aux];			 
		    j++;   
	   }
	}

}


// Finding the ghost pair (returns -1 if node has not a ghost pair)
int ghostPair(int *label_list, int *label_list_inv, int entry){
	int ghost_pair;
	
	if (entry>n_cities-1){
		ghost_pair=label_list[entry];
	}
	else {
		ghost_pair=label_list_inv[entry];		
	}
	return (ghost_pair);
}


// Table code for the reverse solution
int tableCode(int ghost_a, int ghost_b, int ghost_c, int a, int b, int c, int common_a, int common_b, int ghost_flag){
	int ga, gb, gc;
	
	// vertices with degree 2
	if (common_a==1 && common_b==1)
		return -1;
	
	// vertices with degree 3 or 4
	if (ghost_a==-1)
		ga=0;
	else
		ga=1;
	if (ghost_b==-1)
		gb=0;
	else
		gb=1;
	if (ghost_c==-1)
		gc=0;
	else
		gc=1;
		
	if (ga==0 && gb==0 && gc==0){
		if (common_b==1)
			return (a);	
		else
			return (c);
	}
	else if (ga==0 && gb==0 && gc==1){
		return (ghost_c);	
	}
	else if (ga==1 && gb==0 && gc==0){
		return (a);	
	}
	
	if (ghost_flag==0){
		if (gc==0){
			return (c);	
		}
		else {		
			return (ghost_c);	
		}		
	}
	else{
		return (a);	
	}
		
				
}

// correcting the number of entries (removing common paths and assigned components)
// test if simplified graphs outside unfesible candidate component are equal
// Observation: this is equivalent of testing if all entries for a component are grouped
// after removing the feasible components (identified according to testComp) of the 
// list of candidate entries
void simplifyPaths(int *solution_blue_p2, int n_new, int *vector_comp, int *vector_cand, int *n_entries, int n_cand){
	int i, j, k, aux, *comp_seq, *inp_comp_seq;
	
	comp_seq=aloc_vectori(n_new);			// sequence of components for all entries/exits in unfeasible components in the order given by sol_blue
	inp_comp_seq=aloc_vectori(n_cand);		// records the number of entries/exits in each component in comp_seq
	
	// creating comp_seq
	j=0;									// j is the effective size of comp_seq
	k=solution_blue_p2[0];
	aux=vector_cand[k];
	if ( vector_comp[k]==-1){
		if (aux != vector_cand[solution_blue_p2[n_new-1]] ){
			comp_seq[j]=aux;
			j++;	
		}
		if (aux != vector_cand[solution_blue_p2[1]] ){
			comp_seq[j]=aux;		
			j++;	
		}
	}	
	for (i=1;i<n_new-1;i++){
		k=solution_blue_p2[i];
		aux=vector_cand[k];
		if ( vector_comp[k]==-1){			
			if ( aux != vector_cand[solution_blue_p2[i-1] ] ){
				comp_seq[j]=aux;
				j++;
			}
			if (aux != vector_cand[solution_blue_p2[i+1] ] ){
				comp_seq[j]=aux;		
				j++;
			}
		}								
	}
	k=solution_blue_p2[n_new-1];
	aux=vector_cand[k];
	if ( vector_comp[k]==-1){
		if (aux != vector_cand[solution_blue_p2[n_new-2]] ){
			comp_seq[j]=aux;
			j++;	
		}
		if (aux != vector_cand[solution_blue_p2[0]] ){
			comp_seq[j]=aux;		
			j++;	
		}
	}	
	for (i=0;i<n_cand;i++){
		inp_comp_seq[ i ]=0;
	}
	
	// testing by checking the grouping of  the components (i.e., testing if the number of entries is 2)
	if (j>0){
		aux=comp_seq[0];
		if (aux != comp_seq[j-1]){
			inp_comp_seq[ aux ]=inp_comp_seq[ aux ] +1;	
		}
		if (aux != comp_seq[1]){
			inp_comp_seq[ aux ]=inp_comp_seq[ aux ] +1;	
		}
		for (i=1;i<j-1;i++){
			aux=comp_seq[i];
			if (aux != comp_seq[i-1]){		
				inp_comp_seq[ aux ]=inp_comp_seq[ aux ] +1;
			}
			if (aux != comp_seq[i+1]){
				inp_comp_seq[ aux ]=inp_comp_seq[ aux ] +1;
			}
		}	
		aux=comp_seq[j-1];
		if (aux != comp_seq[j-2]){
			inp_comp_seq[ aux ]=inp_comp_seq[ aux ] +1;	
		}
		if (aux != comp_seq[0]){
			inp_comp_seq[ aux ]=inp_comp_seq[ aux ] +1;	
		}	
		for (i=0;i<n_cand;i++){
			if ( n_entries[i]>2 &&  inp_comp_seq[i]==2 ){
				n_entries[i]=2;	
			}
		}		
	}
		
	delete [] inp_comp_seq;
	delete [] comp_seq;
}


// Filling the first columns of the tour table
void tourTable_fill(int **Tour_table, int *d2_vertices, int *solution_blue_p2, int *solution_red_p2, int *solution_red, int *label_list, int *label_list_inv, int *common_edges_blue_p2, int *common_edges_red_p2, int n_new){
	int i, sol1, sol2, ghost_a, ghost_b, ghost_c, common_a, common_b, common_c, a, b, c;
	
	// Inserting in the table the blue and red tours (col. 0-1)
	for (i=0;i<n_new-1;i++){	
		// Inserting in the table the tour for the blue tour
		sol1=solution_blue_p2[i];
		sol2=solution_blue_p2[i+1];		
		if (common_edges_blue_p2[sol1]==0){
			Tour_table[sol1][0]=sol2;
			Tour_table[sol2][0]=sol1;
		}
		else{							
			Tour_table[sol1][3]=sol2;
			Tour_table[sol2][3]=sol1;
		}
		// Inserting in the table the tour for the direct red tour
		sol1=solution_red_p2[i];
		sol2=solution_red_p2[i+1];
		if (common_edges_red_p2[sol1]==0){
			Tour_table[sol1][1]=sol2;
			Tour_table[sol2][1]=sol1;
		}		
	}
	sol1=solution_blue_p2[n_new-1];
	sol2=solution_blue_p2[0];
	if (common_edges_blue_p2[sol1]==0){
		Tour_table[sol1][0]=sol2;
		Tour_table[sol2][0]=sol1;
	}
	else{			
		Tour_table[sol1][3]=sol2;
		Tour_table[sol2][3]=sol1;
	}
	sol1=solution_red_p2[n_new-1];
	sol2=solution_red_p2[0];
	if (common_edges_red_p2[sol1]==0){
		Tour_table[sol1][1]=sol2;
		Tour_table[sol2][1]=sol1;
	}	
		
	// Inserting in the table the reverse red tours (col. 2)	
	a=solution_red[n_cities-1];	
	ghost_a=ghostPair(label_list, label_list_inv, a);
	if (ghost_a==-1)
		common_a=common_edges_red_p2[a];
	else
		common_a=common_edges_red_p2[ghost_a];
	b=solution_red[0];
	ghost_b=ghostPair(label_list, label_list_inv, b);
	if (ghost_b==-1)
		common_b=common_edges_red_p2[b];
	else
		common_b=common_edges_red_p2[ghost_b];
	c=solution_red[1];
	ghost_c=ghostPair(label_list, label_list_inv, c);
	if (ghost_c==-1)
		common_c=common_edges_red_p2[c];
	else
		common_c=common_edges_red_p2[ghost_c];	
	Tour_table[b][2]=tableCode(ghost_a, ghost_b, ghost_c, a, b, c, common_a, common_b, 0);
	if (ghost_b!=-1)
		Tour_table[ghost_b][2]=tableCode(ghost_a, ghost_b, ghost_c, a, b, c, common_a, common_b, 1);
	for (i=1;i<n_cities-1;i++){
		a=b;
		ghost_a=ghost_b;
		common_a=common_b;
		b=c;
		ghost_b=ghost_c;
		common_b=common_c;
		c=solution_red[i+1];		
		ghost_c=ghostPair(label_list, label_list_inv, c);
		if (ghost_c==-1)
			common_c=common_edges_red_p2[c];
		else
			common_c=common_edges_red_p2[ghost_c];
		Tour_table[b][2]=tableCode(ghost_a, ghost_b, ghost_c, a, b, c, common_a, common_b, 0);
		if (ghost_b!=-1)
			Tour_table[ghost_b][2]=tableCode(ghost_a, ghost_b, ghost_c, a, b, c, common_a, common_b, 1);		
	}
	a=b;
	ghost_a=ghost_b;
	common_a=common_b;
	b=c;
	ghost_b=ghost_c;
	common_b=common_c;
	c=solution_red[0];		
	ghost_c=ghostPair(label_list, label_list_inv, c);
	if (ghost_c==-1)
		common_c=common_edges_red_p2[c];
	else
		common_c=common_edges_red_p2[ghost_c];
	Tour_table[b][2]=tableCode(ghost_a, ghost_b, ghost_c, a, b, c, common_a, common_b, 0);
	if (ghost_b!=-1)
		Tour_table[ghost_b][2]=tableCode(ghost_a, ghost_b, ghost_c, a, b, c, common_a, common_b, 1);
	
}


// Identifying the vertices with degree 2
void d2_vertices_id(int *d2_vertices, int *solution_blue_p2,  int *common_edges_blue_p2, int n_new){
	int i;

	if (common_edges_blue_p2[solution_blue_p2[0]]==1 && common_edges_blue_p2[solution_blue_p2[n_new-1]]==1)
		d2_vertices[solution_blue_p2[0]]=1;
	else
		d2_vertices[solution_blue_p2[0]]=0;
	for (i=1;i<n_new;i++){
		if (common_edges_blue_p2[solution_blue_p2[i]]==1 && common_edges_blue_p2[solution_blue_p2[i-1]]==1)
			d2_vertices[solution_blue_p2[i]]=1;
		else
			d2_vertices[solution_blue_p2[i]]=0;
	}
}


// fixing the labels (in order to avoid gaps)
void labelsFix(int *vector_comp, int n_comp, int n_new){
	int i, j, *gap_labels, *new_label;
		
	gap_labels=aloc_vectori(n_comp);
	new_label=aloc_vectori(n_comp);
	for (i=0;i<n_comp;i++){
		gap_labels[i]=1;
		new_label[i]=i;
	}
	for (i=0;i<n_new;i++){
		gap_labels[vector_comp[i]]=0;
	}
	i=0;
	j=n_comp-1;	
	while(j>i){
		while (gap_labels[i]==0)
			i++;
		while(gap_labels[j]==1)
			j--;
		if (j>i){
			new_label[j]=i;	
			gap_labels[i]=0;
			gap_labels[j]=1;
		}		
		i++;
		j--;
	}
	for (i=0;i<n_new;i++){
		vector_comp[i]=new_label[vector_comp[i]];
	}
	
	delete [] new_label;
	delete [] gap_labels;
}


// Finding the connected components using the tours table: one partition each time
void tourTable(int *solution_blue_p2, int *solution_red_p2, int *solution_red, int *label_list, int *label_list_inv, int *vector_comp, int n_new, int *common_edges_blue_p2, int *common_edges_red_p2){
	int i, k, cand_dir, cand_rev, n_comp, sol1, sol2, sol3, sol4, start, edge_tour, ghost_pair, n_rounds=0, n_rounds_max=1000;
	int min_size_dir, min_size_rev, red_chosen, cand_mcuts, cand_mcuts_dir, cand_mcuts_rev, min_size_dir_index, min_size_rev_index;
	int **Tour_table, *assigned_dir, *assigned_rev, *size_dir, *size_rev, *vector_comp_red;
	int *d2_vertices, *visited, *recently_assigned, *entries_flag_rev, *n_entries_dir, *n_entries_rev, *vector_cand_dir, *vector_cand_rev;
	
	// Memory allocation
	d2_vertices=aloc_vectori(n_new);
	visited=aloc_vectori(n_new);					// indicates the visited nodes
	vector_comp_red=aloc_vectori(n_new);			// indicates if ghost pair comes from dir (0) or rev (1) red
	recently_assigned=aloc_vectori(n_new);			// indicates the recently assigned nodes (for reversing ghost nodes)
	entries_flag_rev=aloc_vectori(n_new);				// auxiliary vector used for checking direction of the entries
	vector_cand_dir=aloc_vectori(n_new);				// auxiliary vector for Tour_table (:,4)
	vector_cand_rev=aloc_vectori(n_new);				// auxiliary vector for Tour_table (:,5)
	Tour_table = aloc_matrixi(n_new,6);	// Tours table
										// lines: vertices; 
										// columns: 0 - next single vertex in blue tour, 
										//			 1 - next single vertex in direct red tour, 
										//			 2 - next single vertex in reverse red tour, 
										//			 3 - next common vertex 
										// 			 4 - candidate to connected component following the direct red tour
										// 			 5 - candidate to connected component following the reverse red tour
										// obs.: all vertices has degree 3 or 2										
	
	d2_vertices_id(d2_vertices, solution_blue_p2,  common_edges_blue_p2, n_new);			// identifying the vertices with degree 2
	tourTable_fill(Tour_table, d2_vertices, solution_blue_p2, solution_red_p2, solution_red, label_list, label_list_inv, common_edges_blue_p2, common_edges_red_p2, n_new);		// filling col. 0-3 of the tours table

	// remember that candidates with only one vertex should exist (between common edges)
	// connected components for vertices with degree 2 (each one has a label)
	n_comp=0;
	for (i=0;i<n_new;i++){
		vector_comp_red[i]=-1;		// -1 means that it was not assigned; if assigned, can be 0 (dir. tour) or 1 (rev. tour)
		if (d2_vertices[i]==1 ){
			vector_comp[i]=n_comp;
			n_comp++;
		}
		else
			vector_comp[i]=-1; 		// indicates that vertex i was not assignes yed			
	}
		
								
	// finding the candidates to connected components (AB cycles) with any number of cuts cuts
	do{
		n_rounds++;
		// assigning the components 
		cand_mcuts_dir=0;
		cand_mcuts_rev=0;
		for (i=0;i<n_new;i++){
				if (vector_comp[i]==-1)
					visited[i]=0;
				else
					visited[i]=1;
				Tour_table[i][4]=-1;
				vector_cand_dir[i]=-1;
		}
		
		// folowing direct red tour
		// AB Cycles: direct red tour
		// all assigned become visited				
		cand_dir=0;			
		for (i=0;i<n_new;i++){
			if (visited[i]==0){
				start=i;
				edge_tour=0;		// 0 for blue edge and 1 for red edge
				do{						
					Tour_table[i][4]=cand_dir;
					vector_cand_dir[i]=cand_dir;
					visited[i]=1;					
					if (edge_tour==0){
						i=Tour_table[i][0];			// get blue edge
						edge_tour=1;
					}
					else{
						i=Tour_table[i][1];			// get direct red edge
						edge_tour=0;
					}
				} while (i!=start);
				cand_dir++;				
			}			
		}	
		// finding the number of entries and size	
		n_entries_dir=aloc_vectori(cand_dir);
		assigned_dir=aloc_vectori(cand_dir);
		size_dir=aloc_vectori(cand_dir);	
		for (i=0;i<cand_dir;i++){
			n_entries_dir[i]=0;
			size_dir[i]=0;			
		}
		for (i=0;i<n_new;i++){			
			k=Tour_table[i][4];
			if ( k !=-1 ){
				size_dir[k]=size_dir[k]+1;
				if ( k != Tour_table[ Tour_table[i][3] ][4] ){
					n_entries_dir[k]=n_entries_dir[k]+1;					
				}
			}	
		}					
		simplifyPaths(solution_blue_p2, n_new, vector_comp, vector_cand_dir, n_entries_dir, cand_dir);	// correcting the number of entries (removing common paths and assigned components)
								
		// following reverse red tour
		// AB Cycles: reverse red tour		
		// all assigned become visited
		cand_rev=0;
		for (i=0;i<n_new;i++){
			if (vector_comp[i]==-1)
				visited[i]=0;
			else
				visited[i]=1;
			Tour_table[i][5]=-1;
			vector_cand_rev[i]=-1;	
			entries_flag_rev[i]=0;		// 1 indicates that one of the entries for candidate cand_rev was alredy assigned for direct red tour	(obs.: the effective size is the number of candidates)				
		}
		for (i=0;i<n_new;i++){
			if (visited[i]==0){
				start=i;
				edge_tour=0;		// 0 for blue edge and 1 for red edge
				do{
					Tour_table[i][5]=cand_rev;
					vector_cand_rev[i]=cand_rev;	
					ghost_pair=ghostPair(label_list, label_list_inv, i);						
					if (ghost_pair!=-1){
						//check if i and ghost pair (if exists
						if (vector_comp_red[i]==0 || vector_comp_red[ghost_pair]==0)
							entries_flag_rev[cand_rev]=1;		// 1 indicates that one of the entries for candidate cand_rev was alredy assigned for direct red tour
					}
					visited[i]=1;					
					if (edge_tour==0){
						i=Tour_table[i][0];			// get blue edge
						edge_tour=1;
					}
					else{
						i=Tour_table[i][2];			// get reverse red edge							
						edge_tour=0;
					}
				} while (i!=start);
				cand_rev++;				
			}			
		}	
		// finding the number of entries and size
		n_entries_rev=aloc_vectori(cand_rev);
		assigned_rev=aloc_vectori(cand_rev);
		size_rev=aloc_vectori(cand_rev);
		for (i=0;i<cand_rev;i++){
			n_entries_rev[i]=0;
			size_rev[i]=0;
		}
		for (i=0;i<n_new;i++){
			k=Tour_table[i][5];
			if ( k !=-1 ){
				size_rev[k]=size_rev[k]+1;
				if ( k != Tour_table[ Tour_table[i][3] ][5] ){
					n_entries_rev[k]=n_entries_rev[k]+1;				
				}
			}
		}		
		simplifyPaths(solution_blue_p2, n_new, vector_comp, vector_cand_rev, n_entries_rev,  cand_rev);	// correcting the number of entries (removing common paths and assigned components)
					
		// Assigning the true candidates 
		// new labels for direct red tour
		min_size_dir=n_new;			// minimum size for the candidates
		min_size_dir_index=-1;
		for (i=0;i<cand_dir;i++){
			cand_mcuts_dir++;
			assigned_dir[i]=n_comp;		// new label
			n_comp++;	
			if ( size_dir[i]<min_size_dir || (size_dir[i]==min_size_dir && n_entries_dir[i]==2) )  {
				min_size_dir=size_dir[i];
				min_size_dir_index=i;
			}			
		}
	   // new labels for reverse red tour
	   	min_size_rev=n_new;			// minimum size for the candidates
	   	min_size_rev_index=-1;
	   for (i=0;i<cand_rev;i++){
			if (entries_flag_rev[i]==0){
				cand_mcuts_rev++;
				assigned_rev[i]=n_comp; 	// new label
				n_comp++;
				if (size_rev[i]<min_size_rev || (size_rev[i]==min_size_rev && n_entries_rev[i]==2) ){
					min_size_rev=size_rev[i];
					min_size_rev_index=i;			
				}								
			}
			else{
				assigned_rev[i]=-1;
			}		
		}
		cand_mcuts=cand_mcuts_dir+cand_mcuts_rev;
		if (cand_mcuts>0 && n_rounds<=n_rounds_max){
			// assigning components 
			// choose all components in one tour (only one) that has size equal or smaller than the minimum size of the other component
			// use the number of entries when there is a tie
			if (min_size_rev<min_size_dir)
				red_chosen=1;		// 0 for direct and 1 for reverse
			else if (min_size_rev>min_size_dir)
				red_chosen=0;		// 0 for direct and 1 for reverse
			else {
				// Tie
				if (min_size_dir_index==-1){
					red_chosen=1;		// 0 for direct and 1 for reverse
				}
				else{
					if (min_size_rev_index==-1){
						red_chosen=0;		// 0 for direct and 1 for reverse
					}
					else if(n_entries_dir[min_size_dir_index]==2){	
						red_chosen=0;		// 0 for direct and 1 for reverse					
					}
					else if(n_entries_rev[min_size_rev_index]==2){	
						red_chosen=1;		// 0 for direct and 1 for reverse					
					}
					else if (n_entries_rev[min_size_rev_index]<n_entries_dir[min_size_dir_index]){
						red_chosen=1;		// 0 for direct and 1 for reverse	
					}						
					else{
						red_chosen=0;		// 0 for direct and 1 for reverse
					}								
				}
				
			}			
			for (i=0;i<cand_rev;i++){
				if (assigned_rev[i]!=-1){
					if (red_chosen==0 || size_rev[i]>min_size_dir ) {
						assigned_rev[i]=-1;					
						cand_mcuts_rev--;
					}					
				}
			}
			if (red_chosen==1 && cand_mcuts_rev==0){
				red_chosen=0;
				min_size_rev=min_size_dir;
			}
			for (i=0;i<cand_dir;i++){
				if ( red_chosen==1 || size_dir[i]>min_size_rev ){				
					assigned_dir[i]=-1;
					cand_mcuts_dir--;
				}
			}
			cand_mcuts=cand_mcuts_dir+cand_mcuts_rev;
			if (cand_mcuts>0 ){						
				for (i=0;i<n_new;i++){
					if (vector_comp[i]==-1 ){				
						if (red_chosen==0 && assigned_dir[Tour_table[i][4]]!=-1){
							vector_comp[i]=assigned_dir[Tour_table[i][4]];	// assigning component
							// recording dir. red tour 			
							if 	(vector_comp_red[i]==-1){					
								ghost_pair=ghostPair(label_list, label_list_inv, i);
								if (ghost_pair!=-1){	
									vector_comp_red[i]=0;			// zero means that it comes from the direct red tour																							
									vector_comp_red[ghost_pair]=0;	// zero means that it comes from the direct red tour	
									// reversing the ghost nodes (changing direction in Table) of the red tours
									// exchanging the reverse red edge for i and ghost node
									sol1=i;				
									sol2=ghost_pair;
									sol3=Tour_table[sol1][2];
									sol4=Tour_table[sol2][2];
									Tour_table[sol1][2]=sol4;
									Tour_table[sol4][2]=sol1;
									Tour_table[sol2][2]=sol3;
									Tour_table[sol3][2]=sol2;	
								}
							}																		
						}
						else if (red_chosen==1 && assigned_rev[Tour_table[i][5]]!=-1){
							vector_comp[i]=assigned_rev[Tour_table[i][5]];	// assigning component			
							// recording dir. red tour 			
							if 	(vector_comp_red[i]==-1){					
								ghost_pair=ghostPair(label_list, label_list_inv, i);
								if (ghost_pair!=-1){																								
									vector_comp_red[i]=1;			// zero means that it comes from the direct red tour
									vector_comp_red[ghost_pair]=1;	// zero means that it comes from the direct red tour	
									// reversing the ghost nodes (changing direction in Table) of the red tours
									// exchanging the reverse red edge for i and ghost node
									sol1=i;				
									sol2=ghost_pair;
									sol3=Tour_table[sol1][1];
									sol4=Tour_table[sol2][1];
									Tour_table[sol1][1]=sol4;
									Tour_table[sol4][1]=sol1;
									Tour_table[sol2][1]=sol3;
									Tour_table[sol3][1]=sol2;	
								}
							}	
						}
					}					
				}				
			}	
		}
		else{	
			// When the maximum number of rounds is reached, assign from direct red tour			
			for (i=0;i<cand_dir;i++){
				assigned_dir[i]=n_comp;
				n_comp++;				
			}									
			// assigning new labels 
			for (i=0;i<n_new;i++){
				if (vector_comp[i]==-1 ){								
					vector_comp[i]=assigned_dir[Tour_table[i][4]];	
				}
			}								
		}
				
		delete [] n_entries_dir;	
		delete [] assigned_dir;	
		delete [] size_dir;
		delete [] n_entries_rev;
		delete [] assigned_rev;	
		delete [] size_rev;	
		
	} while (cand_mcuts>0 && n_rounds<=n_rounds_max);  												
				
	labelsFix(vector_comp, n_comp, n_new);			//fixing the labels
	
	// change ghost nodes in red tour	
	for (i=0;i<n_new;i++){
		ghost_pair= ghostPair(label_list, label_list_inv, solution_red_p2[i]);
		if (vector_comp_red[solution_red_p2[i]]==1 && ghost_pair>-1 )
			solution_red_p2[i]=ghost_pair;				
	}
	
	// Desallocating memory
	delete [] vector_comp_red;
	desaloc_matrixi(Tour_table, n_new);
	delete [] d2_vertices;
	delete [] visited;
	delete [] recently_assigned;
	delete [] entries_flag_rev;
	delete [] vector_cand_dir;
	delete [] vector_cand_rev;

}


// GPX2
double gpx(int *solution_blue, int *solution_red, int *offspring )
{
	int i, j,  *d4_vertices,  n_d4_vertices, *common_edges_blue, *common_edges_red;
	int *common_edges_p2_blue, *common_edges_p2_red, *label_list, *label_list_inv, n_new;
	int *solution_blue_p2, *solution_red_p2, *vector_comp, n_newpart;
	double fitness_offspring;
	
	// Step 1: Identifying the vertices with degree 4 and the common edges
	d4_vertices = aloc_vectori(n_cities);
	common_edges_blue = aloc_vectori(n_cities);
	common_edges_red = aloc_vectori(n_cities);
	n_d4_vertices=d4_vertices_id(solution_blue, solution_red, d4_vertices, common_edges_blue, common_edges_red); 

	// Step 2: Insert ghost nodes 	
	n_new=n_cities+n_d4_vertices;				// size of the new solutions: n_cities + number of ghost nodes
	label_list = aloc_vectori(n_new);				// label_list: label for each node (including the ghost nodes)
	label_list_inv = aloc_vectori(n_cities);		// label_list_inv: inverse of label_list
	j=0;										// counter for the vertices with degree 4 (ghost nodes)
	for (i=0;i<n_cities;i++){
		label_list_inv[i]=-1;
		if (d4_vertices[i]==1){
		    label_list[n_cities+j] = i;
		    label_list_inv[i]=n_cities+j;
		    j++;
		}
		label_list[i] = i; 
	}	
	// inserting the ghost nodes in solutions blue and red	
	solution_blue_p2 = aloc_vectori(n_new);				// solution blue with the ghost nodes
	solution_red_p2 = aloc_vectori(n_new);				// solution red with the ghost nodes
	insert_ghost(solution_blue,solution_blue_p2,d4_vertices,label_list_inv);	
	insert_ghost(solution_red,solution_red_p2,d4_vertices,label_list_inv);		
	// identifying the common edges for the new solution
	common_edges_p2_blue = aloc_vectori(n_new);
	common_edges_p2_red = aloc_vectori(n_new);
	j=0;
	for (i = 0; i <n_cities; i++){
		common_edges_p2_blue[i]=common_edges_blue[i];
		common_edges_p2_red[i]=common_edges_red[i];
		if (d4_vertices[i]==1){
			common_edges_p2_blue[i]=1;
			common_edges_p2_red[i]=1;
			common_edges_p2_blue[n_cities+j]=common_edges_blue[i];
			common_edges_p2_red[n_cities+j]=common_edges_red[i];
			j++;
		}
	}		
	
	// Step 3: creating the tour tables and finding the connected components
	vector_comp=aloc_vectori(n_new);					// candidate component for each node (size n_new)	
	tourTable(solution_blue_p2, solution_red_p2, solution_red, label_list, label_list_inv, vector_comp, n_new, common_edges_p2_blue, common_edges_p2_red); // identify connected comp. using tour table
	//compGraph(solution_blue_p2, solution_red_p2, common_edges_p2_blue, common_edges_p2_red , vector_comp, n_new );	// identify connected comp. using graphs

  // Step 4: Creating the candidate components
	candidates *candidate = new candidates(vector_comp, n_new);			// object candidate recombination component
	delete [] vector_comp;
	
	// Step 5: Finding the inputs and outputs of each candidate component
	candidate->findInputs(solution_blue_p2, solution_red_p2);
	//candidate->print();			// print the components

	// Step 6: testing the candidate components
	// Step 6.a: test components using simplified internal graphs
	for (i=0;i<candidate->n_cand;i++){  
		candidate->testComp(i); // test component i 		
	}
	
	// Step 6.b: test unfeasible components using simplified external graphs
	n_newpart=candidate->testUnfeasibleComp(solution_blue_p2);
	
	// Step 7.a: fusions of the candidate components that are neighbours (with more than to cutting points)
	candidate->fusion(solution_blue_p2, solution_red_p2);            // if candidate i did not pass the test and has conditions, apply fusion with the neighbour with more connections        
	candidate->fusion(solution_blue_p2, solution_red_p2);            // if candidate i did not pass the test and has conditions, apply fusion with the neighbour with more connections        
	candidate->fusion(solution_blue_p2, solution_red_p2);            // if candidate i did not pass the test and has conditions, apply fusion with the neighbour with more connections        
	//candidate->fusion(solution_blue_p2, solution_red_p2);            // if candidate i did not pass the test and has conditions, apply fusion with the neighbour with more connections        
	//candidate->fusion(solution_blue_p2, solution_red_p2);            // if candidate i did not pass the test and has conditions, apply fusion with the neighbour with more connections        

	// Step 7.b: fusions of the candidate components in order to create partitions with two cutting points
	candidate->fusionB(solution_blue_p2, solution_red_p2);            // if candidate i did not pass the test and has conditions, apply fusionB to find fusions of partitions in order to have partitions with 2 cutting points        	
	//candidate->print();			// print the components
	
	// Selecting the best between the blue and red path in each component
	fitness_offspring=candidate->off_gen(solution_blue_p2, solution_red_p2, offspring, label_list);			

	
	delete candidate;
	delete [] label_list;
	delete [] label_list_inv;
	delete [] d4_vertices;
	delete [] common_edges_blue;
	delete [] common_edges_p2_blue;
	delete [] common_edges_red;
	delete [] common_edges_p2_red;
	delete [] solution_blue_p2;
	delete [] solution_red_p2;
	
	return fitness_offspring;

}

