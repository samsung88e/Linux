#include "mapreduce.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h> 

#define _BSD_SOURCE

char patharr[MaxNumInputFiles][maxFileNameLength]; //Array to store file paths
int count=0; //Count how many file paths are in a directory

int TraverseInputDir(char* path, int max) 
{
	char * temp;
	struct stat* buf = (struct stat*)malloc(sizeof(struct stat));

	DIR* dir = opendir(path);
	struct dirent* entry;

	//temp variable to store filepath
	char temppath[maxFileNameLength];

	while ((entry = readdir(dir)) != NULL && count < max) 
	{
		strcpy(temppath, path);
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) 
			continue;

	//If its a directory keep going recursively 
		if (entry->d_type == DT_DIR) 
		{
			char next[strlen(path) + strlen(entry->d_name) + 2];
			next[0] = '\0';
			strcat(next, path);
			strcat(next, "/");
			strcat(next, entry->d_name);
			//strcat(temppath, next);  // Get subdirectory: Test/ttt/tt/t
			TraverseInputDir(next, max);
			
			//printf("This is temppath: %s", temppath);		//test
		} else {
			
			strcat(temppath, "/");
			strcat(temppath, entry->d_name);  //Append file name entry->d_name give a string #filename to tempath
			sprintf(patharr[count],"%s\n", temppath);
			
			count++;
		}
	}
	free(buf);
} 


int main(int argc, char *argv[]) 
{	
	int status = 0;
	int temp = 0;
	FILE *fp;
	char  filename[maxFileNameLength];

	////TODO: number of argument check
	if(argc != 4)
	{
		printf("Invalid Argument");
		exit(0);
	}
	int nMappers = strtol(argv[1], NULL, 10);
	int nReducers = strtol(argv[2], NULL, 10);

    inputFileDir = argv[3];
    if(!isValidDir(inputFileDir))
        exit(EXIT_FAILURE);

	bookeepingCode();

	////TODO: Traverse Input File Directory (Hint: Recursively find all the text files inside directory)
	// and create MapperInput directory which will contain MapperID.txt files
	
	//Create MapperInput Directory
	status = mkdir("MapperInput", 0777); 
	
	//Get file path and put into an array
	TraverseInputDir(argv[3], MaxNumIntermediateFiles);
	
	//Create Mapper.txt file in MapperInput directory
	for (int i=0; i<count; i++)
	{
		if(i < nMappers)
		{
			sprintf(filename, "MapperInput/Mapper%d.txt", i);

			fp = fopen(filename,"a");
			writeLineToFile(filename, patharr[i]);
			//fprintf(fp,"%s", patharr[i]);		//test
			fclose(fp);
		}
		else
		{
			temp = i - nMappers;
			
			sprintf(filename, "MapperInput/Mapper%d.txt", temp);			
			
			fp = fopen(filename,"a");

			fprintf(fp,"%s", patharr[i]);
			fclose(fp);
		}
	}

	////TODO: open a 2d Array for pipes file descriptor
	int fd[nMappers][2];

	////TODO: spawn stream processes
	for (int i = 0; i < nMappers; i += 1)
    { 
		pipe(fd[i]);
        pid_t pid = fork();
		
        if (pid == 0)
        {
			printf("child for spawning stream is called");

			close(fd[i][0]);                    // [STDIN -> terminal_input, STDOUT -> terminal_output, fd[1] -> pipe_output] (of the WC process)
        	dup2(fd[i][1], STDOUT_FILENO);      // [STDIN -> pipe_output, STDOUT -> terminal_output, fd[1] -> pipe_output]    (of the WC process)
        	close(fd[i][1]);   	// done writing
			
			//Buffer storing MapperID
            char buf[MaxNumInputFiles]; 
            sprintf(buf, "%d", i);
            // Set up parameters
            char *args[] = {"stream", buf, argv[1], NULL};
			
            // Run the stream with given parameters
            execv(*args, args); 
            // If execv failed, this will be run
            printf("Failed to execute mapper\n");
            exit(EXIT_FAILURE);
        }
		else if(pid >0)
		{
			close(fd[i][1]);
		}
        else if (pid < 0) 
        {
            // Error happened
            perror("Failed to fork");
            exit(EXIT_FAILURE);
        } 
    }
	
	//// TODO: spawn mappers 
	for (int i = 0; i < nMappers; i += 1)
    { 
        pid_t pid = fork();
        if (pid < 0) 
        {
            // Error happened
            perror("Failed to fork");
            exit(EXIT_FAILURE);
        } 
		else if (pid > 0 )
		{
			close(fd[i][0]);
		}
        else if (pid == 0)
        {
			char buf[MaxNumInputFiles]; 
			sprintf(buf, "%d", i+1);
			
            close(fd[i][1]);
			dup2(fd[i][0], STDIN_FILENO);
			close(fd[i][0]);	// done reading
        
			// Set up parameters
            char *args[] = {"mapper", buf, argv[1], NULL};
            // Run the mapper with given parameters
            execv(*args, args); 
            // If execv failed, this will be run
            printf("Failed to execute mapper\n");
            exit(EXIT_FAILURE);
        }
    }

	//// TODO: wait for all children to complete execution
	while (wait(NULL) > 0);

	//// TODO: spawn reducers
	for (int i = 0; i < nReducers; i += 1) 
    {
        pid_t pid = fork();
         if (pid < 0) 
        {
            perror("Failed to fork");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0)  // if it's the child
        {
			//buffer storing reducer ID
            char buf[MaxNumInputFiles];
            sprintf(buf, "%d", i+1);
            
            char *args[] = {"reducer", buf, argv[2], NULL};
            execv(*args, args);
            // If execv failed, this will be run
            printf("File to execute reducer\n");
            exit(EXIT_FAILURE);
        }
    }
	// wait for all children to complete execution
	while (wait(NULL) > 0);

	return EXIT_SUCCESS; 
}
