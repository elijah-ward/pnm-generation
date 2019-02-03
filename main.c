// main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libpnm.h"

#define MAX_GRAY 255

/* function: check_args */

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

/* function: generate_pbm */

int generate_pbm( struct PBM_Image * pbmImage, int width, int height, char* out_filename, int format ) {
    int row, col;
    int quarterWidth = width / 4;
    int quarterHeight = height / 4;
    int colour = 0;

    create_PBM_Image( pbmImage, width, height );

    int isWide = width >= height;
    int strokeStart, strokeEnd, strokeLength;

    strokeStart = 0;

    for ( row = 0; row < pbmImage->height; row++ ) {
        for ( col = 0; col < pbmImage->width; col++ ) {
            if ( row >= quarterHeight && row < (quarterHeight * 3) && col >= quarterWidth && col < (quarterWidth * 3) ) {
                colour = 0;
            } else {
                colour = 1;
            }
            pbmImage->image[row][col] = colour;
        }
    }

    if ( isWide ) {
        strokeLength = width/height;
        strokeEnd = strokeLength;
        for ( row = 0; row < pbmImage->height; row++ ) {
            for ( col = 0; col < pbmImage->width; col++ ) {
                if( col >= strokeStart && col < strokeEnd ) {
                    pbmImage->image[row][col] = 1;
                    pbmImage->image[row][width-col-1] = 1;
                }
            }
            strokeStart = strokeEnd;
            strokeEnd += strokeLength;
        }
    } else {
        strokeLength = height/width;
        strokeEnd = strokeLength;
        for ( col = 0; col < pbmImage->width; col++ ) {
            for ( row = 0; row < pbmImage->height; row++ ) {
                if ( row >= strokeStart && row < strokeEnd ) {
                    pbmImage->image[row][col] = 1;
                    pbmImage->image[height-row-1][col] = 1;
                }
            }
            strokeStart = strokeEnd;
            strokeEnd += strokeLength;
        }
    }

    save_PBM_Image( pbmImage, out_filename, format );
    free_PBM_Image( pbmImage );

    return 0;

}

/* function: generate_pgm */

int generate_pgm( struct PGM_Image * pgmImage, int width, int height, char* out_filename, int format ) {
    int quarterWidth = width / 4;
    int quarterHeight = height / 4;
    int colour = 0;
    int longDim, shortDim;

    if ( width > height ) {
        longDim = width;
        shortDim = height;
    } else {
        longDim = height;
        shortDim = width;
    }

    create_PGM_Image( pgmImage, width, height, MAX_GRAY );

    int isWide = width >= height;

    for ( int row = 0; row < pgmImage->height; row++ ) {
        for ( int col = 0; col < pgmImage->width; col++ ) {
            if ( row >= quarterHeight && row < (quarterHeight * 3) && col >= quarterWidth && col < (quarterWidth * 3) ) {
                colour = MAX_GRAY;
            } else {
                colour = 0;
            }
            pgmImage->image[row][col] = colour;
        }
    }

    if (isWide){
        // top and bottom triangles
        int vEdgeStart, vEdgeEnd, vEdgeLength;
        float vShade, vGradient;

        vEdgeStart = quarterWidth;
        vEdgeLength = width/height;
        vEdgeEnd = vEdgeStart + vEdgeLength;
        vShade = (float) MAX_GRAY;
        vGradient = (float) MAX_GRAY/quarterHeight;

        for ( int row = quarterHeight; row < (quarterHeight * 2); row++ ) {
            for ( int col = quarterWidth; col < (quarterWidth * 2); col++ ) {
                if ( col >= vEdgeStart ) {
                    pgmImage->image[row][width-col-1] = vShade;
                    pgmImage->image[height-row-1][width-col-1] = vShade;
                    pgmImage->image[row][col] = vShade;
                    pgmImage->image[height-row-1][col] = vShade;
                }
            }
            vShade -= vGradient;
            vEdgeStart = vEdgeEnd;
            vEdgeEnd += vEdgeLength;
        }

        // left and right triangles
        float hEdgeStart, hEdgeEnd, fHeight, fWidth;
        float hShade, hGradient, hEdgeLength;

        fHeight = (float) height;
        fWidth = (float) width;
        hEdgeStart = quarterHeight;
        hEdgeLength = fHeight/fWidth;
        hEdgeEnd =  hEdgeStart + hEdgeLength;
        hShade = (float) MAX_GRAY;
        hGradient = (float) MAX_GRAY/quarterWidth;

        for ( int col = quarterWidth; col < (quarterWidth * 2); col++ ) {
            for ( int row = quarterHeight; row < (quarterHeight * 2); row++ ) {
                if ( row >= (int) hEdgeStart ) {
                    pgmImage->image[row][width-col-1] = hShade;
                    pgmImage->image[height-row-1][width-col-1] = hShade;
                    pgmImage->image[row][col] = hShade;
                    pgmImage->image[height-row-1][col] = hShade;
                }
            }
            hShade -= hGradient;
            hEdgeStart = hEdgeEnd;
            hEdgeEnd += hEdgeLength;
        }
    } else {
        // top and bottom triangles
        int vEdgeStart, vEdgeEnd, vEdgeLength;
        float vShade, vGradient;

        vEdgeStart = quarterHeight;
        vEdgeLength = height/width;
        vEdgeEnd = vEdgeStart + vEdgeLength;
        vShade = (float) MAX_GRAY;
        vGradient = (float) MAX_GRAY/quarterWidth;

        for ( int col = quarterWidth; col < (quarterWidth * 2); col++ ) {
            for ( int row = quarterHeight; row < (quarterHeight * 2); row++ ) {
                if ( row >= vEdgeStart ) {
                    pgmImage->image[row][width-col-1] = vShade;
                    pgmImage->image[height-row-1][width-col-1] = vShade;
                    pgmImage->image[row][col] = vShade;
                    pgmImage->image[height-row-1][col] = vShade;
                }
            }
            vShade -= vGradient;
            vEdgeStart = vEdgeEnd;
            vEdgeEnd += vEdgeLength;
        }

        // left and right triangles
        float hEdgeStart, hEdgeEnd, fHeight, fWidth;
        float hShade, hGradient, hEdgeLength;

        fHeight = (float) height;
        fWidth = (float) width;
        hEdgeStart = quarterWidth;
        hEdgeLength = fWidth/fHeight;
        hEdgeEnd =  hEdgeStart + hEdgeLength;
        hShade = (float) MAX_GRAY;
        hGradient = (float) MAX_GRAY/quarterHeight;

        for ( int row = quarterHeight; row < (quarterHeight * 2); row++ ) {
            for ( int col = quarterWidth; col < (quarterWidth * 2); col++ ) {
                if ( col >= (int) hEdgeStart ) {
                    pgmImage->image[row][width-col-1] = hShade;
                    pgmImage->image[height-row-1][width-col-1] = hShade;
                    pgmImage->image[row][col] = hShade;
                    pgmImage->image[height-row-1][col] = hShade;
                }
            }
            hShade -= hGradient;
            hEdgeStart = hEdgeEnd;
            hEdgeEnd += hEdgeLength;
        }
    }

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

