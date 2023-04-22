#include <stdio.h>
#include <stdlib.h>

int NZ = 0;
int NU = 0;
int TZ = 0;
int TU = 0;
int F  = 0;
int parse_params(int argc, char* argv[])
{
        if(argc != 6)
    {
        fprintf(stderr, "Wrong amount of parameters\n");
        return 1;
    }
    for(int i = 2; i < 6; i++)
    {
        int count = atoi(argv[i]);
        if(count < 0 || count > 1000)
        {
            fprintf(stderr, "Wrong time in a parameter\n");
            return 1;
        }
    }
    NZ = atoi(argv[1]);
    NU = atoi(argv[2]);
    TZ = atoi(argv[3]);
    TU = atoi(argv[4]);
    F  = atoi(argv[5]);
    return 0; //no error
}

int main(int argc, char* argv[])
{
    if(parse_params(argc, argv))
    {
        exit(1);
    }
    FILE *file = fopen("proj2.out", "w");
    if(file == NULL)
    {
        exit(1);
    }
    fprintf(file, "Hello World\n");
    printf("Hello World!\n");
}