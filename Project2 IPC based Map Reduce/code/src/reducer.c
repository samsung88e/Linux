#include "reducer.h"

// write the lengths of the words into final folder
void writeFinalDSToFiles(void) 
{
    char line[30];
    char filepath[maxFileNameLength];
    memset(filepath, '\0', maxFileNameLength);

    int i;
    for (i = 0; i < MaxWordLength; i++) 
    {
        if (finalDS[i] > 0) 
        {
            memset(line, '\0', 30);
            sprintf(filepath, "%s/%d.txt", finalDir, (i+1));
            sprintf(line, "%d %d\n", i+1, finalDS[i]);
            writeLineToFile(filepath, line);
        }
    }
}

// For data printing out
void printFinalDS(void) 
{
    printf(" < Final DS Contents >\n");
    int i;
    for (i = 0; i < MaxWordLength; i++) 
    {
        printf("Key: %d - Value: %d\n", (i + 1), finalDS[i]);
    }
}

// get the fild and read by line to reduce the files
void reduce(char * intermediateFileName) 
{
    FILE * fp;                                                      // File pointer
    char *line = (char *)malloc(sizeof(char) * chunkSize);     // The variable line points to a chunk buffer where a new line is stored
    size_t len = chunkSize;                                         // The size of a chunk buffer
    ssize_t nread;                                                  // Length of a new line

    if ((fp = getFilePointer(intermediateFileName)) == NULL) 
    {
        exit(EXIT_FAILURE);
    }
    while((nread = getLineFromFile(fp, line, len)) != -1) 
    {
        ////TODO: Sum up word counts by word length
        int key, value;
        sscanf(line, "%d %d\n", &key, &value);
        finalDS[key - 1] += value;
    }
    free(line);
    fclose(fp);
}

// reduce main 
int main(int argc, char *argv[]) 
{
	if(argc != 3)
    {
		printf("Less number of arguments.\n");
		printf("./reducer reducerID nReducers");
		fflush(stdout);
        exit(EXIT_FAILURE);
	}

	// initialize 
	reducerID = strtol(argv[1], NULL, 10);
	int nReducers = strtol(argv[2], NULL, 10);

    //getReducerTasks function returns a list of intermediate file names that this reducer should process
    char *myTasks[MaxNumIntermediateFiles] = {NULL};
    int nTasks = getReducerTasks(nReducers, reducerID, intermediateDir, &myTasks[0]);

    int tIdx;
    for (tIdx = 0; tIdx < nTasks; tIdx++) 
    {
        printf("reducer[%d] - %s\n", reducerID, myTasks[tIdx]);
        reduce(myTasks[tIdx]);
        free(myTasks[tIdx]);
    }

//    printFinalDS();
    writeFinalDSToFiles();

	return EXIT_SUCCESS;
}