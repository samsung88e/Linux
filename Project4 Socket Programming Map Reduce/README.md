# CSci 4061: Introduction to Operating Systems
# 2021, 04/13 - 05/03

## Project 4 : Socket Programming
## test machine: CSELAB_machines
## date: 05/03/21
## name: Hyunwoo Kim, Oudom So, Frederick Alonzi
## x500: kim00186. doxxx198, alonz021


# This program sets up a word count as in the previous projects through the implemenation of socket programming. The program take an input of a server port through the server side, and input of a file path, number of clients, server IP address, and the server port number. The client uses proccesses to make requests to the server, such as UPDATE WSTAT, which updates the count of word lengths in the file the client is in. The client also logs each request it makes to the server and keeps track of all the counts by reporting them in log.txt. The server uses threads to log the requests recieved by the clients and prints them to the terminal.  

## Messages requested from clinet and listened from server.
# UPDATE_WSTAT: to update PER-FILE word length statistics.
# GET_MY_UPDATES: to get the current number of updates of a client.
# GET_ALL_UPDATES: to get the current number of updates of all clients.
# GET_WSTAT: to get the current word length statistics from the server.

## Hyunwoo Kim: Creating functions for each command on both server and client side as well as debugging the client and server source files and commenting on code to maintain an organized structure

## Frederick Alonzi: Doing interim by setting up connection between client and server and implement GET WSTAT and error handling

## Oudom So: Debugging both client and server source files to create the final working program. 

## Compile

> make clean
> make

## Excution

Server:
./server <Server Port>
$ ./server 8088

Client:
./client <Folder> <# of clients> <Server IP> <Server Port>
$ ./client Testcases/TC2 5 127.0.0.1 8088

## Result
check log.txt and server terminal output

##Notes

No assumptions outside of the assignment were made, no attempt at the extra credit.
