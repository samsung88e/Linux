#include "header.h"

// The master thread initializes the shared queue, 
// result histogram, one producer thread and consumer threads.

// After producer and all consumers complete their work, 
// the master thread writes the final result into
// the output file.

// the master thread needs to check the input arguments 
// and print error messages if argument mismatch. 
// Then it should perform the initialization on shared data structure 
// and launch the producer/-consumers thread. 
// Then the master will wait for all threads to join back and write 
// the final result to one output file "output/result.txt". 
// The output format should be: "%d %d\n", and 
// it will loop through the global histogram from length of 1 to 20.

char *finalDir = "output/result.txt";
char *logDir = "output/log.txt";

int main(int argc, char *argv[])
{
  int consumer_count=atoi(argv[1]);
  pthread_t cond_consumer[consumer_count];
  struct consumer_args* ca[consumer_count];
  pthread_t cond_producer;
  struct Res* rh=(struct Res*) malloc(sizeof(struct Res));
  struct condBuffer* cq=(struct condBuffer*) malloc(sizeof(struct condBuffer));
  struct producer_args* pa=(struct producer_args*) malloc(sizeof(struct producer_args));
  cq->pflag=0; //-p option
  cq->bound=-1; //-b option

  //check the input arguments
  if(argc < 3 || argc > 5)
  {
    printf("Wrong number of args\n");
    exit(1);
  }
  else if (argc == 4)
  {
    if(!strncmp(argv[3],"-p",2))
    {
      cq->pflag = 1;
    }
    else
    {
      printf("argument mismatch\n");
    }
  }
  else if (argc==5)
  {
    cq->cond2 = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(cq->cond2, NULL);
    if(!strncmp(argv[3],"-bp",3))
    {
      cq->pflag=1;
      cq->bound=atoi(argv[4]);
    }
    else if(!strncmp(argv[3],"-b",2))
    {
      cq->bound=atoi(argv[4]);
    }
    else 
    {
      printf("argument mismatch\n");
    }
  }

  bookeepingCode();

  //create log file
  if(cq->pflag==1)
  {
    cq->log = fopen("output/log.txt","w");
    if(cq->log==NULL)
    {
      printf("unable to create log.txt\n");
    }
  }
  //initialization
  cq->q=(struct Queue*) malloc(sizeof(struct Queue));
  cq->q->front=NULL;
  cq->q->rear=NULL;
  cq->q->size=0;
  cq->EOF_flag=0;
  cq->cond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  rh->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  cq->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  pa->buffer=cq;
  pa->f=argv[2];
  pthread_cond_init(cq->cond, NULL);
  pthread_mutex_init(cq->mutex, NULL);
  pthread_mutex_init(rh->mutex, NULL);

  //create consumers and producer threads
  pthread_create(&cond_producer,NULL,producer,(void*) pa);

  for(int i=0;i<consumer_count;i++)
  {
    ca[i]=(struct consumer_args*) malloc(sizeof(struct consumer_args));
    ca[i]->buffer=cq; //shared buffer
    ca[i]->result=rh; //shared result
    ca[i]->consumer_id=i; //consumer id
    pthread_create(&cond_consumer[i],NULL,consumer,(void*) ca[i]);
  }

  //join all the threads
  pthread_join(cond_producer, NULL);
	for (int i=0;i<consumer_count;i++)
  {
    pthread_join(cond_consumer[i], NULL);
  }

  //print result into file result.txt
  FILE *fp;
  fp=fopen("output/result.txt","w");
  if(fp==NULL)
  {
    printf("unable to create result.txt\n");
  }
  for(int i=0; i < MaxWordLength; i++)
  {
    fprintf(fp,"%d %d\n", i+1, rh->letter[i]);
  }

  return 0;
}  

