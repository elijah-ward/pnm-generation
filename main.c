// main.c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int idx;
    for ( idx=0; idx < argc; idx++){
        puts(argv[idx]);
    }
}

