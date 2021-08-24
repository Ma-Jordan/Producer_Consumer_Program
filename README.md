# Producer_Consumer_Program
A program that implements producers and consumers threads to demonstrate multi-threaded processes and how mutex semaphores can be used.

This compiled c code implements multiple producer and consumer threads. When running the code, the command line takes in 3 arguments. 
These are length of time the program should run, the number of producer threads, and the number of consumer threads.
The program will display data about the producer and consumer threads created and data about the buffer's status.
The main program is set on a sleep timer, while the forked off process continually creates and consumes items into the buffer.
Once the main program wakes up, I have it kill the child process and the program terminates.
