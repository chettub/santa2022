# gpx2
This repository contains the code for Generalized Partition Crossover 2 (GPX2). An example of how to recombine two random TSP solutions is presented in the main file.


Description: This code is used to recombine, using GPX2, 2 solutions for the Traveling Salesman Problem. 

Reference:  R. Tinos, D. Whitley, and G. Ochoa (2020). A new generalized partition crossover for the traveling salesman problem: tunneling between local optima. Evolutionary Computation, 28 (2): 255–288.		
Contact: Renato Tinos <rtinos@ffclrp.usp.br>

Running the code: ./gpx2 <name_of_instance.tsp>

Example for running the code with instance xqe3891: 

				make
				
			     ./gpx2 xqe3891.tsp 
			     
Input: i) TSP instance (e.g., xqe3891.tsp); 

Observation: 3 fusion steps are performed (see gpx.cpp). The number of fusion steps can be changed by adding/removing the number of calls for function 
			candidate->fusion(...)

Function gpx: recombine two solutions using GPX2
	
	- Call: fitness_offspring=gpx(parent1,parent2,offspring); 
	
	// where parent1, parent 2, offspring are solution vectors (integer vectors) and fitness_offspring is the fitness of the offspring
		
	- The different steps of gpx2 are commented in function gpx in gpx.cpp
	
	- Class tour.h is used for the manipulation of the candidate recombining partitions in gpx
	
	- Function gpx uses two classes for the manipulation of graphs. Those classes were developed in other projects and are reused here. They contain general (and common) functions for the manipulation of graphs. They can be replaced by other classes (codes) for the manipulation of graphs. For each call in gpx.cpp for one of those functions, a comment was inserted.  
			


	
