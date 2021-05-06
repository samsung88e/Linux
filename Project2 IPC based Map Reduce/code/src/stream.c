#include "stream.h"

////TODO:
/**
 * read lines from input file and write each line to pipes
 * each line will contain words as in Project 1 (Use STDOUT for writing)
 */
void stream(char * inputFileName) 
{
	FILE *file = getFilePointer(inputFileName); 

	fprintf(stderr, "File Path in emit: %s\n", inputFileName);
    if(!file)
    {
       fprintf(stderr, "%s\n", "fileopenerror");    // test
       fprintf(stderr,"%s\n", strerror(errno));
       exit(EXIT_FAILURE);
    } 

	//fprintf(stderr, "%s\n", "beforewhileloop");   // test
    char *line = (char *)malloc(sizeof(char) * chunkSize); 
    size_t len = chunkSize;

	ssize_t nread;
    while((nread=getLineFromFile(file, line, len)) != -1) 
    {   
		strtok(line, "\n"); 
        fprintf(stderr, "The data in Mapperinput: %s\n", line); // test
        fprintf(stdout, "%s ", line);  
    }
    fclose(file);
}
/***
 * Stream main - process will read from the files created by Master
 */
int main(int argc, char *argv[]) 
{
    mapperID = strtol(argv[1], NULL, 10);
    int nMappers = strtol(argv[2], NULL, 10);
    char inputFilename[maxFileNameLength];
    
    sprintf(inputFilename, "MapperInput/Mapper%d.txt", mapperID);
    FILE *file;
    file = getFilePointer(inputFilename);
    
	//fprintf(stderr, "%s\n", inputFilename);
    if(!file)
    {
       printf("file open error\n");
       exit(EXIT_FAILURE);
    } 

    char *line = (char *)malloc(sizeof(char) * chunkSize); 
    size_t len = chunkSize;                                 
	ssize_t nread;
    
    while((nread = getLineFromFile(file, line, len))!=-1) 
    {   
		strtok(line, "\n"); 
		fprintf(stderr, "stdout: %s\n", line);  // test
        stream(line);
    }
    free(line);
    fclose(file);

    return EXIT_SUCCESS;
}
