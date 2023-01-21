### Introduction
We modified GA-EAX [[Nagata, Y., & Kobayashi, S. (2013)](https://doi.org/10.1287/ijoc.1120.0506)] for this competition.
The code is mainly based on [GA-EAX-restart](https://github.com/senshineL/GA-EAX-restart), however we are assuming that the original code was based on the supplementary code of [Nagata, Y., & Kobayashi, S. (2013)](https://doi.org/10.1287/ijoc.1120.0506) as some of the variable names are in Japanese.


### Differences from [GA-EAX-restart](https://github.com/senshineL/GA-EAX-restart)
We have midified [GA-EAX-restart](https://github.com/senshineL/GA-EAX-restart) for this competition to:
* change I/O for the competition
* add path dependant penalty
* allow restart conditions, initial stage, and number of runs to be specified
* allow high precision calculation (64 bit and 128 bit integer cost types)

See [1st place solution](https://www.kaggle.com/competitions/santa-2022/discussion/379167) for detailed changes.

### Usage
```
mkdir bin
cd src
bash compile.sh
cd ../bin
./GA-EAX-restart path_to_tsp_file_or_image.csv [pop_size] [offspring_size] [optimum] [cutoff_time_or_run_times(select run time if negative)] [random_seed] [penalty] [initial_population(optional)] [initial_stage(optional)] [minimum_steps_of_each_stage(optional)] [ratio_minimum_improved_population(optional)]
```
#### Recommended Parameters
pop_size 1000  
offspring_size 300  
optimum 0  
cutoff_time_or_run_times -1 (run once)  
random_seed 42  
penalty 1  

#### Default Parameters
initial_stage 1  
minimum_steps_of_each_stage 200  
ratio_minimum_improved_population 0.01  