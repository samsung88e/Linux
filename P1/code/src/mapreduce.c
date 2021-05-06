#include "mapreduce.h"

// execute executables using execv
void execute(char **argv, int nProcesses){
	pid_t  pid;

	int i;
	for (i = 0; i < nProcesses; i++){
		pid = fork();
		if (pid < 0) {
			printf("ERROR: forking child process failed\n");
			exit(1);
		} else if (pid == 0) {
			char processID[3];
			sprintf(processID, "%d", i+1);
			argv[1] = processID;
			if (execv(*argv, argv) < 0) {
				printf("ERROR: exec failed\n");
				exit(1);
			}
		}
     }
}

int main(int argc, char *argv[]) {
	
	if(argc < 4) {
		printf("Less number of arguments.\n");
		printf("./mapreduce #mappers #reducers inputFileDir\n");
		fflush(stdout);
		exit(0);
	}

	int nMappers 	= strtol(argv[1], NULL, 10);
	int nReducers 	= strtol(argv[2], NULL, 10);

    inputFileDir = argv[3];
    if(!isValidDir(inputFileDir))
        exit(EXIT_FAILURE);

	if(nMappers == 0 || nReducers == 0){
		printf("ERROR: Mapper and Reducer count should be grater than zero...\n");
		fflush(stdout);
        exit(EXIT_FAILURE);
	}

    if(nMappers > MaxNumProcesses || nReducers > MaxNumProcesses){
        printf("ERROR: Mapper and Reducer count should be less than MaxNumProcesses(20)...\n");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

	bookeepingCode();

	// spawn mappers
	char *mapperArgv[] = {"mapper", NULL, argv[1], inputFileDir, NULL};
	execute(mapperArgv, nMappers);

	// wait for all children to complete execution
    while (wait(NULL) > 0);

	// spawn reducers
	char *reducerArgv[] = {"reducer", NULL, argv[2], NULL};
	execute(reducerArgv, nReducers);

	// wait for all children to complete execution
    while (wait(NULL) > 0);

	return EXIT_SUCCESS;
}