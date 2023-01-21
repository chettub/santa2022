### usage
```
mkdir bin
cd src
bash compile.sh
cd ../bin
./GA-EAX-restart path_to_tsp_file_or_image.csv [pop_size] [offspring_size] [optimum] [cutoff_time_or_run_times(select run time if negative)] [random_seed] [penalty] [initial_population(optional)] [initial_stage(optional)] [minimum_steps_of_each_stage(optional)] [ratio_minimum_improved_population(optional)]
```
#### recommended parameters
pop_size 1000  
offspring_size 300  
optimum 0  
cutoff_time_or_run_times -1 (run once)  
random_seed 42  
penalty 1  

#### default parameters
initial_stage 1  
minimum_steps_of_each_stage 200  
ratio_minimum_improved_population 0.01  
