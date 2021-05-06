#include "header.h"
/*
 *The consumers will read from the shared queue, 
 compute the “word length statistics” for its data
pieces and synchronize the result with a global histogram.
*/ 

// consumer function
void *consumer(void* args)
{
  struct consumer_args* ca=(struct consumer_args*) args;
  struct QNode* node= (struct QNode*) malloc(sizeof(struct QNode));
  if(ca->buffer->pflag==1)
  {
    fprintf(ca->buffer->log,"consumer %d\n",ca->consumer_id);
  }
  
  while(1)
  {
    pthread_mutex_lock(ca->buffer->mutex);
    while(ca->buffer->q->size==0)
    {
      if(ca->buffer->EOF_flag==1)
      {
        //if reach the end_of_file, signal the waiting consumers
        //unlock and exit the thread
        pthread_cond_signal(ca->buffer->cond);
        pthread_mutex_unlock(ca->buffer->mutex);
        if(ca->buffer->pflag==1)
        {
          fprintf(ca->buffer->log,"consumer %d: -1\n", ca->consumer_id);
        }
        pthread_exit(NULL);
      }
      //if the queue is empty but the EOF flag is not set
      pthread_cond_wait(ca->buffer->cond, ca->buffer->mutex);
    }
    node=deQueue(ca->buffer->q);

    if(ca->buffer->pflag == 1)
    {
      fprintf(ca->buffer->log,"consumer %d: %d\n",ca->consumer_id, node->line_num);
    }
    if(ca->buffer->bound > 0)
    {
      //if we set the bound signal the producer
      pthread_cond_signal(ca->buffer->cond2);
    }
    pthread_mutex_unlock(ca->buffer->mutex);

    char *str=node->line;
    int count[MaxWordLength]={};

    char* token;
    char* rest = str;
    while (token = strtok_r(rest, " \n", &rest))
    {  
      count[strlen(token) - 1]++;
    }
    
    //put count into the shared result histogram
    pthread_mutex_lock(ca->result->mutex);

    for(int i=0; i < MaxWordLength; i++)
    {
      ca->result->letter[i] = ca->result-> letter[i] + count[i];
    }
    pthread_mutex_unlock(ca->result->mutex);
    }

  return 0;
}
    


