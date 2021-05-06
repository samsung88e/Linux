# Project 1 : MapReduce - Word counts of different lengths
# 2021, 02/09 - 24
＊ CSci4061 S2021 Assignment 1
＊ login: samsung88e
＊ date: 02,24, 2021
＊ name: Hyunwoo Kim, Oudom So, Freddy Alonzi
＊ id: 5419240(kim00186), 5327529(Oudom526), 5466000(alonz021)

# Oudom So assigned - Classifying overall framwork and job done
## Master function (mapreduce.c) 
 # The master process drives all the other phases in the project. It takes three inputfrom the user: number of mappers, number of reducers and the path of the input text file relative to the provided Makefile location.

# Hyunwoo Kim assigned - dibugging and solving errors 
## Map function (mapper.c) 
# The mapper takes in three inputs. The mapper’s id (i.e. 1, 2, ...). This will be assigned by the master when it calls exec on the mapper executable

# Freddy Alonzi assigned - Modifying and putting the codes together 
## Reduce function (reducer.c) 
# The reducer takes in three inputs. The reducer’s id (i.e., 1, 2,..). This will be assigned by the master when it calls the exec on the reducer executable similar to the mapper. The other parameters are total number of reducers and intermediate file directory path.

## Compile
	> make clean
	> make

## Execution
	// always call make clean and make before you start execution
	// ./mapreduce nMappers nReducer inputFileDir
	> ./mapreduce 5 3 test/T0

## Result
Check output folder for all the results
	
## Note:
The current code is the first draft. There might be some edge case that I missed, like the empty file case. I am working on them. The code is just for giving you an idea of how the overall project looks like.

The header files are not required as the function definition is in the same file as usage. This is the layout that I usually follow.

