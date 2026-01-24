#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "main.h"

int main(int argc, char const *argv[])
{
    int c = getopt(argc, (char * const *)argv, "v");
    switch (c)
    {
    case 'v':
        printf("demo mode %s\n", VERSION);
        exit(0);
    default:
        break;
    }
    printf("Hello world!\n");
    return 0;
}
