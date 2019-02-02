// main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libpnm.h"

#define MAX_GRAY 255

int check_args(int type, int width, int height, char *out_filename, int format) {

    int status = 0;

    /* CHECK TYPE */

    if ( type <= 0 || type > 3 ) {
        puts("Error: image type code must be either 1, 2, or 3.");
        status = 1;
    }

    /* CHECK WIDTH */

    // If we have PBM or PGM format...
    if ( type == 1 || type == 2 ) {
        // Check if image width is at least 4 and is divisible by 4
        if ( !(width % 4 == 0 && width / 4 >= 1 ) ) {
            puts("Error: for pbm and pgm formats, image width must be at least 4 and a multiple of 4");
            status = 1;
        }
    } else if ( type == 3 ) {
        // If we have PPM then we check that image width is at least 6 and divisible by 6
        if ( !(width % 6 == 0 && width / 6 >= 1 ) ) {
            puts("Error: for pbm and pgm formats, image width must be at least 6 and a multiple of 6");
            status = 1;
        }
    }

    /* CHECK HEIGHT */
    // Check if image height is at least 4 and is divisble by 4
    if ( !(height % 4 == 0 && height / 4 >= 1 ) ) {
        puts("Error: for all formats, image height must be at least 4 and a multiple of 4");
        status = 1;
    }

    /* CHECK FORMAT */

    if ( format != 0 && format != 1) {
        puts("Error: format must be either 0, for ASCII, or 1, for raw");
        status = 1;
    }

    if (status) {
        puts("One or more errors occurred while executing. Exiting...");
        return 1;
    }

    return 0;

}

int generate_pbm( struct PBM_Image * pbmImage, int width, int height, char* out_filename, int format ) {

    create_PBM_Image( pbmImage, width, height );
    save_PBM_Image( pbmImage, out_filename, format );
    free_PBM_Image( pbmImage );

    return 0;

}

int generate_pgm( struct PGM_Image * pgmImage, int width, int height, char* out_filename, int format ) {

    create_PGM_Image( pgmImage, width, height, MAX_GRAY );
    save_PGM_Image( pgmImage, out_filename, format );
    free_PGM_Image( pgmImage );

    return 0;

}

int generate_ppm( struct PPM_Image * ppmImage, int width, int height, char* out_filename, int format ) {

    create_PPM_Image( ppmImage, width, height, MAX_GRAY );
    save_PPM_Image( ppmImage, out_filename, format );
    free_PPM_Image( ppmImage );

    return 0;

}

int main(int argc, char **argv) {

    int type, width, height, format;
    char *out_filename;

    struct PBM_Image pbmImage;
    struct PGM_Image pgmImage;
    struct PPM_Image ppmImage;

    for ( int idx = 0; idx < argc; idx++) {
        puts(argv[idx]);
    }

    type = atoi(argv[1]);
    width = atoi(argv[2]);
    height = atoi(argv[3]);
    out_filename = argv[4];
    format = atoi(argv[5]);

    // INPUT VALIDATION FUNC
    int e = check_args(type, width, height, out_filename, format);
    if (e) {
        exit(0);
    }

    switch(type) {
        case 1:
            generate_pbm( &pbmImage, width, height, out_filename, format );
            break;
        case 2:
            generate_pgm( &pgmImage, width, height, out_filename, format );
            break;
        case 3:
            generate_ppm( &ppmImage, width, height, out_filename, format );
            break;
    }

}

