#include <stdio.h>
#include <stdlib.h>
#include <time.h> //rand
#include <unistd.h> //usleep
#include <stdarg.h> //va_list
#include <sys/types.h> //pid_t
#include <semaphore.h> //sem_t
#include <sys/mman.h> //mmap
#include <semaphore.h>
#include <stdbool.h>
#include <sys/wait.h>

int NZ = 0;     //number of customers
int NU = 0;     //number of workers
int TZ = 0;     //customer wait time
int TU = 0;     //worker wait time
int F  = 0;     //post office open time

FILE *file;

sem_t *queue_service1;
sem_t *queue_service2;
sem_t *queue_service3;

sem_t *write_file;
sem_t *action;      //for iteration of A
int *A;
bool *closed;       //0 default, 1 after post closes
sem_t *closed_sem;
sem_t *mutex_q1;
sem_t *mutex_q2;
sem_t *mutex_q3;
sem_t *customer_sem;
sem_t *worker_sem;
int *q1;
int *q2;
int *q3;

void semaphores_init()
{
    write_file = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(write_file, 1, 1);

    A = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *A = 0;

    closed = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *closed = 0;
    closed_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(closed_sem, 1, 1);

    action = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(action, 1, 1);

    queue_service1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(queue_service1, 1, 0);
    queue_service2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(queue_service2, 1, 0);
    queue_service3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(queue_service3, 1, 0);

    mutex_q1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(mutex_q1, 1, 1);

    mutex_q2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(mutex_q2, 1, 1);

    mutex_q3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(mutex_q3, 1, 1);


    q1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *q1 = 0;
    q2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *q2 = 0;
    q3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *q3 = 0;

    customer_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(customer_sem, 1, 0);
    
    worker_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(worker_sem, 1, 0);
}

//TODO: cleanup
void cleanup()
{
    sem_destroy(write_file);
    munmap(write_file, sizeof(sem_t));
    munmap(A, sizeof(int));
    sem_destroy(action);
    munmap(action, sizeof(sem_t));

    sem_destroy(queue_service1);
    munmap(queue_service1, sizeof(sem_t));
    sem_destroy(queue_service2);
    munmap(queue_service2, sizeof(sem_t));
    sem_destroy(queue_service3);
    munmap(queue_service3, sizeof(sem_t));

    munmap(closed, sizeof(bool));
    sem_destroy(closed_sem);
    munmap(closed_sem, sizeof(sem_t));
    munmap(q1, sizeof(int));
}

void print_flush(const char * format, ...)
{
    sem_wait(write_file);
    va_list args;
    va_start (args, format);
    sem_wait(action);
    fprintf(file, "%d: ", ++*A);
    sem_post(action);
    vfprintf (file, format, args);
    fprintf(file, "\n");
    fflush(file);
    va_end (args);
    sem_post(write_file);
}

int parse_params(int argc, char* argv[])
{
    if(argc != 6)
    {
        fprintf(stderr, "Wrong amount of parameters\n");
        return 1;
    }
    int err = sscanf(argv[1], "%d", &NZ);
    if(err < 1 || err == EOF)
        return 1;
    
    err = sscanf(argv[2], "%d", &NU);
    if(err < 1 || err == EOF)
        return 1;
    
    if(NU < 1)
    {
        fprintf(stderr, "Wrong amount in a parameter NU\n");
        return 1;
    }

    err = sscanf(argv[3], "%d", &TZ);
    if(err < 1 || err == EOF)
        return 1;
    
    if(TZ < 0 || TZ > 10000)
    {
        fprintf(stderr, "Wrong time in a parameter TZ\n");
        return 1;
    }
    err = sscanf(argv[4], "%d", &TU);
    if(err < 1 || err == EOF)
        return 1;
    
    if(TU < 0 || TU > 100)
    {
        fprintf(stderr, "Wrong time in a parameter TU\n");
        return 1;
    }
    
    err = sscanf(argv[5], "%d", &F);
    if(err < 1 || err == EOF)
        return 1;

    if(F < 0 || F > 10000)
    {
        fprintf(stderr, "Wrong time in a parameter F\n");
        return 1;
    }
    return 0; //no error
}

void enlist_queue(int service)
{
    switch(service)//enlists quee X and waits for call from worker
    {
        case 1:
            sem_wait(mutex_q1);
            *q1 = *q1 + 1;
            sem_post(mutex_q1);
            sem_post(queue_service1);
            break;
        case 2:
            sem_wait(mutex_q2);
            *q2 = *q2 + 1;
            sem_post(mutex_q2);
             sem_post(queue_service2);
            break;
        case 3:
            sem_wait(mutex_q3);
            *q3 = *q3 + 1;
            sem_post(mutex_q3);
            sem_post(queue_service3);
            break;
    }
}

int customer(int idZ)
{
    print_flush("Z %d: started", idZ);
    if(TZ != 0)
    {
        srand(time(NULL) * getpid());
        int sleep_time = rand() % TZ;
        if(sleep_time != 0)
            usleep(sleep_time*1000);
    }
    sem_wait(closed_sem);   
    if(*closed)
    {
        sem_post(closed_sem);
        print_flush("Z %d: going home", idZ);
        exit(0);
    }
    else
    {
        sem_post(closed_sem);


        srand(time(NULL) * getpid());
        int service = (rand() % 3) + 1;
        print_flush("Z %d: entering office for a service %d", idZ, service);
        
        enlist_queue(service);
        
        sem_wait(worker_sem);
        print_flush("Z %d: called by office worker", idZ);
 
        srand(time(NULL) * getpid());
        int sleep_time = rand() % 10;
        if(sleep_time != 0)
            usleep(sleep_time*1000);
                
        print_flush("Z %d: going home", idZ);
        exit(0);
    }
}

int pick_queue(int idU)
{
    int service = 0;
    sem_wait(mutex_q1);
    sem_wait(mutex_q2);
    sem_wait(mutex_q3);
    if(*q1 == 0 && *q2 == 0 && *q3 == 0)
    {
        sem_wait(closed_sem);
        if(*closed)
        {
            sem_post(closed_sem);            
            print_flush("U %d: going home", idU);
            sem_post(mutex_q1);
            sem_post(mutex_q2);
            sem_post(mutex_q3);
            exit(0);    
        }
        else
        {
            sem_post(closed_sem);

            print_flush("U %d: taking break", idU);
            srand(time(NULL) * getpid());
            int sleep_time = rand() % 10;
            if(sleep_time != 0)
                usleep(sleep_time* 1000);
            print_flush("U %d: break finished", idU);
            service = -1;
        }
    }
    else if(*q1 > 0)
    {
        *q1 = *q1 - 1;
        service =  1;
        sem_wait(queue_service1);
    }
    else if(*q2 > 0)
    {
        *q2 = *q2 - 1;
        service = 2;
        sem_wait(queue_service2);
    }
    else if(*q3 > 0)
    {
        *q3 = *q3 - 1;
        service = 3;
        sem_wait(queue_service3);
    }
    sem_post(mutex_q1);
    sem_post(mutex_q2);
    sem_post(mutex_q3);
    return service;
}

int worker(int idU)
{
    print_flush("U %d: started", idU);
    while(1)
    {
        srand(time(NULL) * getpid());
        int sleep_time = rand() % 10;
        if(sleep_time != 0)
            usleep(sleep_time * 1000);

        int service = pick_queue(idU);
        if(service == -1)
            continue;
        
        sem_post(worker_sem);
        print_flush("U %d: serving a service of type %d", idU, service);

        srand(time(NULL) * getpid());
        sleep_time = rand() % 10;
        if(sleep_time != 0)
            usleep(sleep_time*1000);
    

        print_flush("U %d: service finished", idU);
    }
    exit(1);
}

int main(int argc, char* argv[])
{
    if(parse_params(argc, argv))
    {
        exit(1);
    }
    file = fopen("proj2.out", "w");
    if(file == NULL)
    {
        exit(1);
    }
    semaphores_init();
    int idZ = 0;
    while(idZ < NZ)
    {
        idZ++;
        pid_t pid = fork();
        if(pid == 0)
        {
            customer(idZ);
        }
    }
    int idU = 0;
    while(idU < NU)
    {
        idU++;
        pid_t pid = fork();
        if(pid == 0)
        {
            worker(idU);
        }
    }
    if(F != 0)
        usleep(F*1000);
    sem_wait(closed_sem);
    *closed = 1;
    sem_post(closed_sem);
    
    
    if(F != 0)
    {
        srand(time(NULL) * getpid());
        int sleep_time = rand() % F/2 + F/2;
        usleep(sleep_time*1000);
    }
    
    while(wait(NULL) > 0);
    print_flush("closing");
    cleanup();
    return 0;
}