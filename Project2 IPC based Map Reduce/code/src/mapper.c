#include "mapper.h"

/**
 * Write word count that is stored in an intermediate data structure to files.
 * The file should be in a corresponding folder in output/IntermediateData/ 
 */
void writeInterDSToFiles(void) 
{
    char line[30];
    char filepath[maxFileNameLength];
    memset(filepath, '\0', maxFileNameLength);

    int i;
    for (i = 0; i < MaxWordLength; i++) 
    {
        if (interDS[i] > 0) 
        {
            memset(line, '\0', 30);
            sprintf(line, "%d %d\n", i+1, interDS[i]);
            sprintf(filepath, "%s/%d/m%d.txt", intermediateDir, (i+1), mapperID);
            writeLineToFile(filepath, line);
        }
    }
}

 ////TODO:
 /*
 * parse lines from pipes, and count words by word length
 */
void parse(char *line) 
{
    char* token;
    char* rest = line;
    while ((token = strtok_r(rest, " ", &rest))) 
    {
        interDS[strlen(token) - 1]++;
    }
}

// Get a inputfile and parse by each line
void map(char * inputFileName) 
{                                       
    char *line = (char *)malloc(sizeof(char) * chunkSize);  // The variable line points to a chunk buffer where a new line is stored
    size_t len = chunkSize;                                 // The size of a chunk buffer
    ssize_t nread;                                          // Length of a new line

    while((nread=read(STDIN_FILENO, line, len)) > 0) 
    {   
        strtok(line, "\n"); 
        parse(line); 
    }
    free(line);
}

// mapper main
int main(int argc, char *argv[]) 
{
    mapperID = strtol(argv[1], NULL, 10);
    char *line = (char *)malloc(sizeof(char) * chunkSize);  // The variable line points to a chunk buffer where a new line is stored
    size_t len = chunkSize;                                 // The size of a chunk buffer
    ssize_t nread;   

    while((nread=getLineFromFile(stdin, line, len)) != -1) 
    {   
		strtok(line, "\n"); 
        fprintf(stderr, "The data is: %s\n", line); // test
        parse(line);
    }

    //    printInterDS();
    writeInterDSToFiles();

    return EXIT_SUCCESS;
}

