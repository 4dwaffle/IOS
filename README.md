# IOS - synchronization project
(IOS = operating systems class at BUT FIT)

## Task description (Post office)
We have 3 types of processes in the system: **(0) main process**, **(1) postal clerk**, and **(2) customer**. Each customer goes to the post office to handle one of three types of requests: letter services, parcel services, money services. Each request is uniquely identified by a number (letters:1, parcels:2, money services:3). On arrival, they are queued according to the activity to be handled. Each clerk serves all the queues (picking one of the queues at random each time). If there are no customers currently waiting, the clerk takes a short break. After the post office closes, the clerks finish serving all customers in the queue and go home when all queues are empty. Any customers who arrive after the post office closes go home (tomorrow is also a day).

## Detailed task specification
Each process performs its actions and simultaneously writes information about the actions to a file named 
proj2.out. The action output information includes the sequence number **A** of the action being executed (see Example output). Actions are numbered from one. 

### Startup:

$ ./proj2 NZ NU TZ TU F

- **NZ**: number of customers
- **NU**: number of clerks
- **TZ**: maximum time in milliseconds a customer waits after being created before entering the
    0 <= TZ <= 10 000
- **TU**: The maximum length of a clerk's pause in milliseconds. 0 <= TU <= 100
- **F**: Maximum time in milliseconds after which the mail is closed to new arrivals.
    0 <= F <= 10 000

### Main process

- Ceates NZ customer processes and NU clerk processes immediately after start
- Waits a random time in the interval <F/2,F> by calling usleep
- Prints: "*A: closing*"
- Then waits for closure of all processes that the application creates. Once these processes are
    are terminated, the main process is terminated with exit code 0.

### Process Customer

- Each customer is uniquely identified by idZ, 0 < idZ <= NZ
- Prints "*A: Z idZ: started*" after start
- Then waits a random time in the interval <0,TZ>
- If the post office is closed
    - Prints "*A: Z idZ: going home*"
    - Process ends
- If the post office is open, randomly selects an X (number activity from the interval <1,3>)
    - Print: "*A: Z idZ: entering office for a service X*"
    - Queues X and waits to be called by a clerk
    - Print: "*Z idZ: called by office worker"*
    - Then waits a random time in the interval <0,10> (synchronization with clerk to complete the request is not required).
    - Prints "*A: Z idZ: going home*"
    - Process ends

### Process clerk

- Each clerk is uniquely identified by the number idU, 0 < idU <= NU
- prints: "*A: U idU: started*" when started [start of the cycle].
- The clerk goes to serve the customer from queue X (a random non-empty queue)
    - Prints "*A: U idU: serving a service of type X*"
    - Then waits a random time in the interval <0,10>
    - Print "*A: U idU: service finished*"
    - Continues to [start of the cycle]
- If there is no customer waiting in any queue and post office is open
    - Print !*A: U idU: taking break*"
    - Then waits a random time in the interval <0,TU>
    - Print "*A: U idU: break finished*"
    - Continues to [start of the cycle]
- If there is no customer waiting in any queue and the post office is closed
    - Print "*A: U idU: going home*"
    - Process ends



## Example output

Example output file proj2.out for the ommand 

$ ./proj2 3 2 100 100 100

>1: U 1: started\
2: Z 3: started\
3: Z 1: started\
4: Z 1: entering office for a service 2\
5: U 2: started\
6: Z 2: started\
7: Z 3: entering office for a service 1\
8: Z 1: called by office worker\
9: U 1: serving a service of type 2\
10: U 1: service finished\
11: Z 1: going home\
12: Z 3: called by office worker\
13: U 2: serving a service of type 1\
14: U 1: taking break\
15: closing\
16: U 1: break finished\
17: U 1: going home\
18: Z 2: going home\
19: U 2: service finished\
20: U 2: going home\
21: Z 3: going home