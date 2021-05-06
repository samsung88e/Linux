#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include "../include/protocol.h"

/*test machine: CSELAB CSEL-KH4250-08
* name: Hyunwoo Kim , Oudom So, Frederick Alonzi
* x500: kim00186. doxxx198, alonz021 */

int client = 0;
pthread_mutex_t currentConn_lock;
int wstat[26];
int updateStatus[32][3];

// Stucture to store thread function arguments
struct threadArg {
	int clientfd;
	char * clientip;
	int clientport;
};

// Function to handle client requests
void * threadFunctionUpdate(void * arg) {
	int curr_signal = -1;
	int new_signal = -1;
	int index = 0;
	int sum = 0;
	int request[28] = {0};
	int response[3] = {0};
	struct threadArg * tArg = (struct threadArg *) arg;
	int break_flag = 0;

	while(1) {
		// Read request from client
		int nn = read(tArg->clientfd, &request, 28*sizeof(int));
		if (nn == 0) break;
		// Store request
		new_signal = request[0];
		  if(new_signal != curr_signal || new_signal == UPDATE_WSTAT) {
			// If new request is sent or if the request is UPDATE_WSTAT,
			// continue to switch statement
			switch (request[0]) {
				case UPDATE_WSTAT:
					// Server sums the word count results from WSTAT, and increments
					// update field by 1 in updateStatus table for client client
					pthread_mutex_lock(&currentConn_lock);
					// If client ID is less than 1, than send NOK response
					if(request[1] < 1) {
						response[0] = UPDATE_WSTAT;
						response[1] = RSP_NOK;
						response[2] = request[1];
						write(tArg->clientfd, response, 3 * sizeof(int));
						break;
					}
					// Increment number of updates field for current client
					updateStatus[index][1]++;
					// sum the word count results
					for(int i=2; i<23; i++) {
						wstat[i-2] += request[i];
					}
					response[0] = UPDATE_WSTAT;
					response[1] = RSP_OK;
					response[2] = request[1];
					pthread_mutex_unlock(&currentConn_lock);
					write(tArg->clientfd, response, 3 * sizeof(int));
					break;
				case GET_WSTAT:
					// The server returns the current value of WSTAT

					// Handle master client request
					if(request[1] == -1) {
						printf("[%d] GET_WSTAT\n",request[1]);
						int i = 2;
						while(i < 24) {
							response[i] = wstat[i-2];
							i++;
						}
						response[0] = GET_WSTAT;
						response[1] = RSP_OK;
						write(tArg->clientfd, response, 28 * sizeof(int));
						printf("close connection from %s:%d\n",tArg->clientip, tArg->clientport);
						close(tArg->clientfd);
						pthread_exit(NULL);
						break;
					}

					printf("[%d] GET_WSTAT\n",request[1]);
					// Copy WSTAT into response array
					for(int i=2; i<24; i++) {
						response[i] = wstat[i-2];
					}
					response[0] = GET_WSTAT;
					response[1] = RSP_OK;
					write(tArg->clientfd, response, 28 * sizeof(int));
					break;
				case GET_MY_UPDATES:
					// Server returns current value of number of updates field
					// for current client

					// If client id is less than one and it is not the master,
					// send NOK response

					if(request[1] < 1 && request[1] != -1) {
						response[0] = GET_MY_UPDATES;
						response[1] = RSP_NOK;
						write(tArg->clientfd, response, 3 * sizeof(int));
						break;
					}
					// Handle Master request
					if(request[1] == -1) {
						printf("[%d] GET_MY_UPDATES\n",request[1]);
						printf("client PID (-1) should be greater than 0\n");
						response[0] = GET_MY_UPDATES;
						response[1] = RSP_NOK;
						write(tArg->clientfd, response, 3 * sizeof(int));
						printf("close connection from %s:%d\n",tArg->clientip, tArg->clientport);
						close(tArg->clientfd);
						pthread_exit(NULL);
						break;
					}

					response[0] = GET_MY_UPDATES;
					response[1] = RSP_OK;
					response[2] = updateStatus[index][1];
					printf("[%d] GET_MY_UPDATES\n",request[1]);
					write(tArg->clientfd, response, 3 * sizeof(int));
					break;
				case GET_ALL_UPDATES:
					// Server returns the sum of all values of number of updates field

					// Handle Master client request
					if(request[1] == -1) {
						for(int i=0; i<32; i++) {
							sum += updateStatus[i][1];
						}
						response[0] = GET_ALL_UPDATES;
						response[1] = RSP_OK;
						response[2] = sum;
						printf("[%d] GET_ALL_UPDATES\n",request[1]);
						printf("close connection from %s:%d\n",tArg->clientip, tArg->clientport);
						write(tArg->clientfd, response, 3 * sizeof(int));
						close(tArg->clientfd);
						pthread_exit(NULL);
						break;
					}

					// Sum the value of all updates from updateStatus table
					for(int i=0; i<32; i++) {
						sum += updateStatus[i][1];
					}

					response[0] = GET_ALL_UPDATES;
					response[1] = RSP_OK;
					response[2] = sum;
					printf("[%d] GET_ALL_UPDATES\n",request[1]);
					write(tArg->clientfd, response, 3 * sizeof(int));
					break;

				default:
					// Send NOK response if request is not recognized
					// printf("Error: Invalid request\n");
					response[0] = -1;
					response[1] = RSP_NOK;
					response[2] = -1;
					write(tArg->clientfd, response, 3 * sizeof(int));
					break;
			}
			if(break_flag == 1) break;
		}
		// Update the signal response
		curr_signal = new_signal;
	}
	return NULL;
}


int main(int argc, char *argv[]) {

	int server_port;

	if (argc == 2) { // 1 argument
		server_port = atoi(argv[1]);
	} else {
		printf("Invalid or less number of arguments provided\n");
		printf("./server <server Port>\n");
		exit(0);
	}

	// Server (Reducer) code
	pthread_t threads[MAX_CONCURRENT_CLIENTS];
	pthread_mutex_init(&currentConn_lock, NULL);

	// Create a TCP socket.
	int sock = socket(AF_INET , SOCK_STREAM , 0);

	// Bind it to a local address.
	struct sockaddr_in servAddress;
	servAddress.sin_family = AF_INET;
	servAddress.sin_port = htons(server_port);
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	int b = bind(sock, (struct sockaddr *) &servAddress, sizeof(servAddress));
	if(b<0) {
		printf("Failure to bind!\n");
		exit(0);
	}
	// We must now listen on this port.
	int l = listen(sock, MAX_CONCURRENT_CLIENTS);
	if(l<0) {
		printf("Failure to listen!\n");
		exit(0);
	}
	printf("server is listening\n");
	int thread_id = 0;
	// A server typically runs infinitely, with some boolean flag to terminate.
	while (1) {
		// Now accept the incoming connections.
		struct sockaddr_in clientAddress;

		socklen_t size = sizeof(struct sockaddr_in);
		int clientfd = accept(sock, (struct sockaddr*) &clientAddress, &size);
		if(clientfd<0) {
			printf("Failure to accept\n");
			exit(0);
		}

		struct threadArg *arg = (struct threadArg *) malloc(sizeof(struct threadArg));

		arg->clientfd = clientfd;
		arg->clientip = inet_ntoa(clientAddress.sin_addr);
		arg->clientport = clientAddress.sin_port;
		printf("open connection from %s:%d\n",arg->clientip, arg->clientport);

		// Create the thread
		pthread_create(&threads[thread_id], NULL, &threadFunctionUpdate, (void*)arg);
		thread_id++;
	}

	// Close the socket.
	close(sock);


	return 0;
}
