#include <stdio.h>
#include <stdlib.h>
#include <time.h> //rand
#include <unistd.h> //usleep
#include <stdarg.h> //va_list
#include <sys/types.h> //pid_t
#include <semaphore.h> //sem_t
#include <sys/mman.h> //mmap
#include <semaphore.h>

int NZ = 0;
int NU = 0;
int TZ = 0;
int TU = 0;
int F  = 0;

FILE *file;

sem_t *post;
sem_t *write_file;
int *A;
sem_t *action;

void semaphores_init()
{
    post = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(post, 1, 1);

    write_file = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(write_file, 1, 1);

    A = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    A = 0;

    action = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(action, 1, 1);
}


void cleanup()
{
    sem_destroy(post);
    munmap(post, sizeof(sem_t));
    munmap(write_file, sizeof(sem_t));
    munmap(A, sizeof(int));
    munmap(action, sizeof(sem_t));
}

void print_flush(const char * format, ...)
{
    //sem_wait(write_file);
    va_list args;
    va_start (args, format);
    vfprintf (file, format, args);
    fprintf(file, "\n");
    fflush(file);
    va_end (args);
    //sem_post(write_file);
}

int parse_params(int argc, char* argv[]) //todo přepsat atoi sscanf a checkovat -1
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
    while(idZ <= NZ)
    {
        idZ++;
        pid_t pid = fork();
        if(pid == 0)
        {
            print_flush("%d: Z %d: started", ++A, idZ);
            srand(time(NULL));
            usleep(rand() % TZ);
            //sem_wait(post);
                //print_flush("A: %d: going home", idZ);
            //else
                srand(time(NULL));
                
                int service = rand() % 2;
                service++;
                sem_wait(action);
                print_flush("%d: Z %d: entering office for a service %d", ++A, idZ, service);
                sem_post(action);
                
                //enlists quee X and waits for call from officer
                
                sem_wait(action);
                print_flush("%d: Z %d: called by office worker", ++A, idZ);
                sem_post(action);

                srand(time(NULL));
                int sleep_time = rand() % 10;
                sleep(sleep_time);
                //print_flush("sleep_time = %d", sleep_time);
                
                sem_wait(action);
                print_flush("%d: Z %d: going home", ++A, idZ);
                sem_post(action);
            return 0;
        }
    }
    srand(time(NULL));
    usleep(rand() % F/2 + F/2);
    
    sem_wait(action);
    print_flush("%d: closing\n", ++A);
    sem_post(action);
    //waits for end of all proceses
    cleanup();
    exit(0);
}