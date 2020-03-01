## Thread Synchronization
#### Author: Saina Rezvani, Josh Desmond
###### Date: 02/29/2020

This project includes the main.c, main.h, queue.c, queue.h, people_threads.c, people_threads.h, probability.c, probability.h, 
shallow_queue.c, shallow_queue.h, thread_demo.c, thread_demo.h files.

### File/Phase Descriptions: 

The program initially takes the inputs from  command line or a test file and performs error checking to make sure they are valid. In the next step all the persons, pirate or ninja are initialized as a person with a thread and are put into a queue. Additionally, we produce a fitting a time and arrival time for each person by producing a variance with a normal probability which is then added to the average values for arrival and fitting time. Each person, after creation, sleeps for the period of arrival time and  enters another queue (a shallow queue) after. In this case we use a semaphore so two people can't enter the queue at the same time. If there is no one in the fitting room, the state of the program is unlocked and the person with the highest waiting time in the queue enters the fitting room. During this time a mutex lock is put on the queue. If a person is already in fitting room we check the waiting time of the people in the queue. If the waiting time of the person of the other kind is significantly larger then we wait for the fitting room to be become empty and we bring the other person into fitting room. If the difference is not very significant then we add the person of the same kind to the fitting room with anothe team to utalize the most of the team/processing time. There is 25% chance a person comes back. In that case the person hangsout outside and repeats the process. 


### Outputs:

The outputs of running our test file "test.sh" are recorded in the output file, test_output.txt.


### Running the files:

##### Running Thread Synchronization:

$ make
$ ./main 4 12 10 7 5 10 4


##### Running Tests:

$ make
$ sh test.sh


##### Makefile includes multiple files, but main.c is the final and correct version of the Thread Synchronization program.
