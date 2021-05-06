# CSci 4061: Introduction to Operating Systems
# 2021, 03/21 - 04/14

## Project 3 : Multithreading

Use multithreading to create a producer thread to read the file and multiple consumer threads to process 
the smaller piece data. 
We will have two forms of synchronization: a shared queue synchronized between producer and consumers,
and a global result histogram synchronized by consumers.
The program contains: a master thread, one producer thread and many consumer threads 
# For program execution flow, the entire program contains 4 parts:
1. The master thread initializes the shared queue, result histogram, one producer thread and consumer
threads.

2. The producer thread will read the input file, cut the data into smaller pieces and feed into the shared
queue.

3. The consumers will read from the shared queue, compute the “word length statistics” for its data
pieces and synchronize the result with a global histogram.

4. After producer and all consumers complete their work, the master thread writes the final result into
the output file.


# Hyunwoo Kim: Assigned writing main and creating the data structure such as each struct and Queue.
Also debugged other code in consumer.c and producer.c

# Frederick Alonzi: Assigned with doing the interim, and starting on the base layout for main

# Oudom So: Assigned with designating the tasks and starting on the base layout for consumer 
and producer. Also debugged consumer, main and producer to create the final files. 

## Master  (main.c)
	Creates the producer and consumer threads and designates their tasks. It then joins the 
	threads and prints all the final results to the output file.
## Consumer  (consumer.c)
	Consumer uses an infinite while loop to constantly check for when something is added
	to the queue by the producer. It also updates the log of each loop while it is going through. 
	The continues to read until an EOF notification.

## Producer  (producer.c)
	The producer reads data from the files and then puts that data into the nodes and then 
	adds nodes to the queue using enqueue and writes to the log each action it makes.
	Once the producer is done, it sends an EOF flag to the consumer to let it know that it is done.

## Compile
	> make clean
	> make

## Execution
	// always call make clean and make before you start execution
	// ./mapreduce nConsumers inputFileDir option queue size
	> ./mapreduce 10 test/T0/F0.txt -bp 20

## Result
Check output folder for all the results

## Testing
You can manually check test folder and compare your results in
Final Output with expected folder.

## Note
For any custom test case, make sure you end the file with new line character
	

