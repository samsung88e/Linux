# Project 2 : IPC MapReduce - Word counts of different lengths


## Master (mapreduce.c)
- Handles spawning the directory traversal, spawning MapInput directory and textfiles that stores the path to each files. 
- Spawns stream, mapper and reducer with fork and exec. 


## Stream (stream.c)
- Reads in mapper.txt files with getLineFromFile() and send each line to emit()
- emit takes in a file path and opens the file in that path then reads the contents of that file and writes it to stdout

  

## Map (mapper.c)
- Mapper reads in from stdin the contents of the files sent by the emit function in stream
- Mapper then counts the words sent in the data and writes them to the intermediate data structure

  

## Reduce (reducer.c)
- The reducer takes in three inputs. The reducer's id (i.e.,1, 2,..). This will be assigned by the master when it calls the exec on the reducer executable similar to the
mapper. The other parameters are total number of reducers and intermediate file directory path
- The reducer first gets its tasks using getReducerTasks and list of intermediate file names, then loops through the
tasks and calls reduce on each one. Reduce reads in the line from the file using getLineFromFile and stores it
using strtok and stores each token in finalDS
- writeFinalDSToFiles is then called which is similar to the one in mapper. The function combines the count and length in to CountandLength and then writes it to the
corresponding text file for word length. 

  

## Utils (utils.c)
- Contains function such as getLineFromFile(), WriteLinetoFile(), getReducerTasks(), and bookkeepingcode() for creating ouput directory,
- getLineFromFile, WriteLinetoFile() and bookkeepingcode() are given in the utils.c.
- getReducerTrasks return the number of task the reducer needs to do; hence, the amount of textfile.


## Role Delegation

Oudom:
- Mapreduce Function and directory traversal in the getreducetask and mapreduce.
- Setup the directory traversal in the mapreduce function and create the MapperInput Directories and filled with the filepath from the traverse directory function
- Set up pipes and fork for stream and mapper, as well as fork for the reducer.

Hyunwoo:
- Code Stream.c, Mapper.c, util.c(getReducerTasks and getMapperTasks function) work peroperly.
- Put together with other member's codes in one.
- Debugging and problem solve betwwen the codes from members.

Freddy:
- Mapreduce function traversal
- Setting up pipes in stream.c
- Debugged getReducerTasks to correctly send intermediate files to myTasks

## Compile

  make:
  lib/utils.o mapreduce mapper reducer stream


  make clean:
  rm lib/utils.o mapreduce mapper reducer stream
	
  
## Execution

always call make clean and make before you start execution

./mapreduce nMappers nReducer inputFileDir

> ./mapreduce 5 3 test/T0

  

## Result

Check output folder for all the results
Reducer - Finaldir and Mapper - IntermediateDir

  
## Testing

You can manually check test folder and compare your results in

Final Output with expected folder.

Also, check the MapperInput folder, it should be similar to

that in expected. It was generated for Test case 1.

  

## Note

For any custom test case, make sure you end the file with new line character