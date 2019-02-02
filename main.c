// main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int flags, opts;
    int type, height, width, format;
    char **output_name;

    int type, width, height, format;
    char *out_filename;

    struct PBM_Image pbmImage;
    struct PGM_Image pgmImage;
    struct PPM_Image ppmImage;

    for ( int idx = 0; idx < argc; idx++) {
        puts(argv[idx]);
    }
}

