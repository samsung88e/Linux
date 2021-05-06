#include "header.h"
// pthread.h included in header.h

// Feel free to add any functions or global variables

/* File operations */
void writeLineToFile(char *filepath, char *line) {
    int fd = open(filepath, O_CREAT | O_WRONLY | O_APPEND, 0777);
    if (fd < 0){
        printf("ERROR: Cannot open the file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    int ret = write(fd, line, strlen(line));
    if(ret < 0){
        printf("ERROR: Cannot write to file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
}

FILE * getFilePointer(char *inputFileName) {
    return fopen(inputFileName, "w");
}

ssize_t getLineFromFile(FILE *fp, char *line, size_t len) {
    memset(line, '\0', len);
    return getline(&line, &len, fp);
}

void _removeOutputDir(){
    pid_t pid = fork();
    if(pid == 0){
        char *argv[] = {"rm", "-rf", "output", NULL};
        if (execvp(*argv, argv) < 0) {
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
}

void bookeepingCode()
{
    _removeOutputDir();
    sleep(1);
    _createOutputDir();
}

void enQueue(struct Queue* q,struct QNode* new_node)
{
  new_node->next=NULL;

  if(q->rear==NULL)
  {
    q->front=new_node;
    q->rear=new_node;
  }
  else
  {
    q->rear->next=new_node;
    q->rear=new_node;
  }
  q->size=q->size+1;
}

struct QNode* deQueue(struct Queue* q)
{
  struct QNode* temp;
  if(q->front==NULL)
  {
    return NULL;
  }
  temp=q->front;
  q->front=temp->next;

  if(q->front==NULL)
  {
    q->rear=NULL;
  }
  q->size=q->size-1;
  
  return temp;
}