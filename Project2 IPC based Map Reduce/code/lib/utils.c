#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void writeLineToFile(char *filepath, char *line) 
{
    int fd = open(filepath, O_CREAT | O_WRONLY, 0777);
    if (fd < 0)
    {
        printf("ERROR: Cannot open the file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    int ret = write(fd, line, strlen(line));
    if(ret < 0)
    {
        printf("ERROR: Cannot write to file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
}

FILE * getFilePointer(char *inputFileName) {
    return fopen(inputFileName, "r");
}

ssize_t getLineFromFile(FILE *fp, char *line, size_t len) {
    memset(line, '\0', len);
    return getline(&line, &len, fp);
}

////TODO:
// return: The number of intermediate files the reducer should process
// The list of intermediate file names are stored in myTasks array
int getReducerTasks(int nReducers, int reducerID, char *intermediateDir, char **myTasks) 
{
	int countt = 0;
    struct dirent *entry;
    //char *intermediateDir = "output/IntermediateData"; 1~20
   
    for(int i = reducerID; i < MaxNumProcesses; i++)
    {
    	//char* filePath = malloc(strlen(intermediateDir) + strlen(entry->d_name) + 2 * sizeof(char*));
    	char filePath[maxFileNameLength];
    	sprintf(filePath, "%s/%d", intermediateDir, i);
    	DIR *dir = opendir(filePath);
    	 if (dir == NULL)
    	{
        	fprintf(stderr, "opendir() error!\n");
        	exit(EXIT_FAILURE);
    	}
    while ((entry = readdir(dir)) != NULL) 
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
        { continue; }

       char* file = malloc(maxFileNameLength * sizeof(char));
       sprintf(file, "%s/%s", filePath, entry->d_name);
       myTasks[countt] = file;
       countt++;
    }
       closedir(dir);
    }
    return countt;
}

int isValidDir(char *folder) 
{
    struct stat sb;
    return (stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode));
}

void _removeOutputDir()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        char *argv[] = {"rm", "-rf", "output", NULL};
        if (execvp(*argv, argv) < 0) 
        {
            printf("ERROR: exec failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else{
        wait(NULL);
    }
}

void _createOutputDir()
{
    mkdir("output", ACCESSPERMS);
    mkdir("output/IntermediateData", ACCESSPERMS);
    mkdir("output/FinalData", ACCESSPERMS);
}

void _createInterFolders(){
    int wordLen;
    for(wordLen = 1; wordLen <= MaxWordLength; wordLen++) 
    {
        char dirName[50];
        memset(dirName, '\0', 50);
        sprintf(dirName, "output/IntermediateData/%d", wordLen);
        mkdir(dirName, ACCESSPERMS);
    }
}

void bookeepingCode()
{
    _removeOutputDir();
    sleep(1);
    _createOutputDir();
    _createInterFolders();
}



