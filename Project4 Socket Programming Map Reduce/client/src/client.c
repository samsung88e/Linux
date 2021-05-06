#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <zconf.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../include/protocol.h"

/*test machine: CSELAB CSEL-KH4250-08
* date: 05/03/21
* name: Hyunwoo Kim , Oudom So, Frederick Alonzi
* x500: kim00186. doxxx198, alonz021 */

FILE *logfp;

// Initialize request array with size of 28
int request[28];
void recursiveTraverseFS(int clients, char *basePath, FILE *fp[], int *toInsert, int *nFiles){
	//check if the directory exists
	DIR *dir = opendir(basePath);
	if(dir == NULL){
		printf("Unable to read directory %s\n", basePath);
		exit(1);
	}

	char path[1000];
	struct dirent *dirContentPtr;

	while((dirContentPtr = readdir(dir)) != NULL){
		if (strcmp(dirContentPtr->d_name, ".") != 0 &&
			strcmp(dirContentPtr->d_name, "..") != 0){
			struct stat buf;
			 lstat(path, &buf);
			 if (S_ISLNK(buf.st_mode))
				 continue;
			if (dirContentPtr->d_type == DT_REG){
				//file
				char filePath[1000];
				strcpy(filePath, basePath);
				strcat(filePath, "/");
				strcat(filePath, dirContentPtr->d_name);

				//insert into the required client file
				fputs(filePath, fp[*toInsert]);
				fputs("\n", fp[*toInsert]);

				*nFiles = *nFiles + 1;

				//change the toInsert to the next client file
				*toInsert = (*toInsert + 1) % clients;
			}else if (dirContentPtr->d_type == DT_DIR){
				//directory
				// basePath creation - Linux platform
				strcpy(path, basePath);
				strcat(path, "/");
				strcat(path, dirContentPtr->d_name);
				recursiveTraverseFS(clients, path, fp, toInsert, nFiles);
			}
		}
	}
}

void traverseFS(int clients, char *path){
    FILE *fp[clients];

    pid_t p = fork();
    if (p==0)
        execl("/bin/rm", "rm", "-rf", "MapperInput", NULL);

    wait(NULL);
    //Create a folder 'ClientInput' to store Mapper Input Files
    mkdir("MapperInput", ACCESSPERMS);

    // open client input files to store paths of files to be processed by each client
    int i;
    for (i = 0; i < clients; i++){
        // create the mapper file name
        char mapperCount[10];
        sprintf(mapperCount, "%d", i + 1);
        char mapInFileName[100] = "MapperInput/Mapper_";
        strcat(mapInFileName, mapperCount);
        strcat(mapInFileName, ".txt");
        fp[i] = fopen(mapInFileName, "w");

    }
    //refers to the File to which the current file path should be inserted
    int toInsert = 0;
    int nFiles = 0;
    recursiveTraverseFS(clients, path, fp, &toInsert, &nFiles);

    // close all the file pointers
    for(i = 0; i < clients; i++)
        fclose(fp[i]);

    if(nFiles == 0){
        pid_t p1 = fork();
        if (p1==0)
            execl("/bin/rm", "rm", "-rf", "MapperInput", NULL);
    }

    if (nFiles == 0){
        printf("The %s folder is empty\n", path);
        exit(0);
    }
}

// Function to count the first letter of the word
int count_letter(char * arrLine, int client_id, int sockfd) {
  FILE * fp;

  // Get rid of new line
  if (arrLine[strlen(arrLine) - 1] == '\n') {
    arrLine[strlen(arrLine) - 1] = '\0';
  }

  // open the txt file to count
  fp = fopen(arrLine, "r");
  if (fp == NULL)
  exit(EXIT_FAILURE);

  // Put all zero into the request array
  int request[28] = {0};
  // Put all zero into the response array
  int response[RESPONSE_MSG_SIZE] = {0};
  // Assign 'UPDATE_WSTAT' as Request Command
  request[0] = UPDATE_WSTAT;
  // Assign client id
  request[1] = client_id;

  // Array to the current line in the file
  char line[1024];

  // While there are still lines in the file, continue reading
  while(fgets(line, sizeof(line), fp) != NULL) {
    char* token;
    char* rest = line;
    while ((token = strtok_r(rest, " \n", &rest)))
    {
      request[strlen(token)+2]++;
    }
  }

  // Send request to server
  write(sockfd, request, 28 * sizeof(int));

  // Get response from server
  read(sockfd, response, RESPONSE_MSG_SIZE * sizeof(int));

  // Close the txt file
  fclose(fp);
  return 0;
}

// Function to get a path of txt file from Mapper.txt
void get_path_from_txt(int client_id, int sockfd) {
  FILE * fp;
  char * line = NULL;
  char * arrLine[1000];
  char txtPath[40];
  size_t len = 0;
  ssize_t read;

  // Assign correct name of the path
  sprintf(txtPath, "MapperInput/Mapper_%d.txt", client_id);

  // Open the client file
  fp = fopen(txtPath, "r");
  if (fp == NULL)
  exit(EXIT_FAILURE);

  int i = 1;

  // Define number of messages to send to server
  int num_of_msg = 0;

  // Read txt file line by line
  while ((read = getline(&line, &len, fp)) != -1) {
    arrLine[i] = line;
    // Call count function to count the correct txt file
    count_letter(arrLine[i], client_id, sockfd);
    i++;
    // Increase the number of messages every time
    num_of_msg++;
  }

  // Print log messages
  fprintf(logfp, "[%d] UPDATE_WSTAT: %d\n", client_id, num_of_msg);

  fclose(fp);
  if (line){
    free(line);
  }
}

// Send this request to the server with PER-FILE word count results
void update_wstat(int client_id, int sockfd) {
  get_path_from_txt(client_id, sockfd);
}

// If mapper clients want to send this GET_WSTAT request
// to the server, they should be already checked in
void get_WSTAT(int client_id, int sockfd) {
  int request[28] = {0};
  int response[28] = {0};
  char src[1000];
  char count[1000];
  request[0] = GET_WSTAT;
  request[1] = client_id;
  // Send request to server
  write(sockfd, request, 28 * sizeof(int));
  // Get response from server
  read(sockfd, response, 28 * sizeof(int));
  sprintf(src, "[%d] GET_WSTAT: %d", client_id, response[RSP_RSP_CODE_NUM]);
  for (int i=3; i<23; i++) {

    sprintf(count, " %d", response[i]);
    strcat(src, count);
  }

  fprintf(logfp, "%s", src);
  fprintf(logfp, "\n");
}

// Only client clients can send this request and
// they should be already checked in
void get_my_updates(int client_id, int sockfd) {
  int request[28] = {0};
  int response[RESPONSE_MSG_SIZE] = {0};
  request[0] = GET_MY_UPDATES;
  request[1] = client_id;
  // Send request to server
  write(sockfd, request, 28 * sizeof(int));
  // Get response from server
  read(sockfd, response, RESPONSE_MSG_SIZE * sizeof(int));
  fprintf(logfp, "[%d] GET_MY_UPDATES: %d %d\n", client_id, response[RSP_RSP_CODE_NUM], response[RSP_DATA]);
}

// If client clients want to send this GET_WSTAT request
// to the server, they should be already checked in
void get_all_updates(int client_id, int sockfd) {
  int request[28] = {0};
  int response[RESPONSE_MSG_SIZE] = {0};
  request[0] = GET_ALL_UPDATES;
  request[1] = client_id;
  // Send request to server
  write(sockfd, request, 28 * sizeof(int));
  // Get response from server
  read(sockfd, response, RESPONSE_MSG_SIZE * sizeof(int));

  fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", client_id, response[RSP_RSP_CODE_NUM], response[RSP_DATA]);
}

// After getting a response, the client client closes its
// TCP connection and terminates its own process

// Function that have all the request function
void mapperFunction(int client_id, int sockfd) {
  update_wstat(client_id, sockfd);
  get_WSTAT(client_id, sockfd);
  get_my_updates(client_id, sockfd);
  get_all_updates(client_id, sockfd);
	fprintf(logfp, "[%d] close connection\n", client_id);
	close(sockfd);
}

// Function to create a log file
void createLogFile(void) {
  pid_t p = fork();
  if (p==0)
  execl("/bin/rm", "rm", "-rf", "log", NULL);

  wait(NULL);
  mkdir("log", ACCESSPERMS);
  logfp = fopen("log/log_client.txt", "w");
}

// Function to create new socket
int newsocket(int server_port, char *server_ip) {

  // Define new socket
  int newsock = socket(AF_INET , SOCK_STREAM , 0);

  // Specify an address to connect to
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_port = htons(server_port);
  address.sin_addr.s_addr = inet_addr(server_ip);

  // Connect it.
  if (connect(newsock, (struct sockaddr *) &address, sizeof(address)) != 0) {
    printf("New socket -- Connection Fail!\n");
  }
  fprintf(logfp, "[-1] open connection\n");

  return newsock;
}

int main(int argc, char *argv[]) {
  int clients;
  char folderName[100] = {'\0'};
  char *server_ip;
  int server_port;

  if (argc == 5) { // 4 arguments
    strcpy(folderName, argv[1]);
    clients = atoi(argv[2]);
    server_ip = argv[3];
    server_port = atoi(argv[4]);
    if (clients > MAX_NUM_CLIENTS) {
      printf("Maximum number of clients is %d.\n", MAX_NUM_CLIENTS);
      printf("./client <Folder Name> <# of clients> <server IP> <server Port>\n");
      exit(1);
    }

    if (clients < 1) {
      printf("Not enough clients.\n");
      printf("./client <Folder Name> <# of clients> <server IP> <server Port>\n");
      exit(1);
    }

  } else {
  	if(argc < 5) {
    		printf("Too few arguments provided\n");
    		printf("./client <Folder Name> <# of clients> <server IP> <server Port>\n");
    		exit(1);
    	}
    	else{
    		printf("Too many arguments provided\n");
    		printf("./client <Folder Name> <# of clients> <server IP> <server Port>\n");
    		exit(1);
    	}
  }

  // Create log file
  createLogFile();

  traverseFS(clients, folderName);

  for (int client_id = 1; client_id <= clients; client_id++) {
    pid_t pid = fork();

    // child process
    if (pid == 0) {

      // Create a TCP socket.
      int sockfd = socket(AF_INET , SOCK_STREAM , 0);

      // Specify an address to connect to
      struct sockaddr_in address;

      address.sin_family = AF_INET;
      address.sin_port = htons(server_port);
      address.sin_addr.s_addr = inet_addr(server_ip);

      // printf("before connected\n");
      // Connect it.
      if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) != 0) {
        printf("Connection Fail!\n");
      }

      fprintf(logfp, "[%d] open connection\n", client_id);

      // Socket connected successfully and
      // call client function to start work each request
      mapperFunction(client_id, sockfd);

      exit(0);
    }
  }

  // Every client should wait to run at the same time to make it sequential
  for (int client_id = 1; client_id <= clients; client_id++) {
    wait(NULL);
  }


  // Close the log_client.txt
  fclose(logfp);
  return 0;
}
