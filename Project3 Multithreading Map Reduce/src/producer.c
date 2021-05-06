#include "header.h"
// The producer thread will read the input file,
// cut the data into smaller pieces and feed into the shared
// queue.

/**
 *
 * Producer thread will read from the file and write data to 
 * the end of the shared queue
 */
void *producer(void *args)
{
    struct producer_args* pa=(struct producer_args*)args;
    FILE* fp=fopen(pa->f,"r");
    int count=0;//count the line number
    char str[chunkSize];
    
    if(pa->buffer->pflag==1)
    {
        fprintf(pa->buffer->log,"producer\n");
    }
    if (fp == NULL) 
    {
        printf("Could not open file %s\n", pa->f);
        exit(1);
    }

    while(getLineFromFile(fp, str, chunkSize) != -1)
    {
        //create new node for queue
        struct QNode* new_node= (struct QNode*) malloc(sizeof(struct QNode));
        new_node->line_num=count;
        new_node->line=(char *)malloc(sizeof(char)*1024);
        strcpy(new_node->line, str);
        //put the node into queue
        pthread_mutex_lock(pa->buffer->mutex);

    if(pa->buffer->bound>0)
    {//if we have set the bound
        while(pa->buffer->q->size>=pa->buffer->bound)
        {
        //check the size of the queue
        pthread_cond_wait(pa->buffer->cond2,pa->buffer->mutex);
        }
    }

    enQueue(pa->buffer->q,new_node);

    if(pa->buffer->pflag==1)
    {
        fprintf(pa->buffer->log,"producer: %d\n",count);
    }
    count++;
    pthread_cond_signal(pa->buffer->cond);
    pthread_mutex_unlock(pa->buffer->mutex);
    }
    
    //change the end_of_file flag
    pa->buffer->EOF_flag=1;
    //if all consumers are in the wait queue before we
    //set the flag, signal at least one consumer
    pthread_cond_signal(pa->buffer->cond);

    return 0; 
}





