#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "libpnm.h"

#define MAX_GRAY 255

/**
 * @brief      { check_args }
 *
 * @param[in]  type          The type
 * @param[in]  width         The width
 * @param[in]  height        The height
 * @param      out_filename  The out filename
 * @param[in]  format        The format
 *
 * @return     { returns integer 1 if validation fails, else 0 }
 */

int check_args(int type, int width, int height, char *out_filename, int format)
{

    int status = 0;

    /* CHECK TYPE */

    if ( type <= 0 || type > 3 )
    {
        puts("Error: image type code must be either 1, 2, or 3.");
        status = 1;
    }

    /* CHECK WIDTH */

    // If we have PBM or PGM format...
    if ( type == 1 || type == 2 )
    {
        // Check if image width is at least 4 and is divisible by 4
        if ( !(width % 4 == 0 && width / 4 >= 1 ) )
        {
            puts("Error: for pbm and pgm formats, image width must be at least 4 and a multiple of 4");
            status = 1;
        }
    }
    else if ( type == 3 )
    {
        // If we have PPM then we check that image width is at least 6 and divisible by 6
        if ( !(width % 6 == 0 && width / 6 >= 1 ) )
        {
            puts("Error: for pbm and pgm formats, image width must be at least 6 and a multiple of 6");
            status = 1;
        }
    }

    /* CHECK HEIGHT */
    // Check if image height is at least 4 and is divisble by 4
    if ( !(height % 4 == 0 && height / 4 >= 1 ) )
    {
        puts("Error: for all formats, image height must be at least 4 and a multiple of 4");
        status = 1;
    }

    /* CHECK FORMAT */

    if ( format != 0 && format != 1)
    {
        puts("Error: format must be either 0, for ASCII, or 1, for raw");
        status = 1;
    }

    if (status)
    {
        puts("One or more errors occurred while executing. Exiting...");
        return 1;
    }

    return 0;

}

/**
 * @brief      { generate_pbm }
 *
 * @param      pbmImage      The portable bitmap image
 * @param[in]  width         The width
 * @param[in]  height        The height
 * @param      out_filename  The out filename
 * @param[in]  format        The format
 *
 * @return     { void }
 */

void generate_pbm( struct PBM_Image *pbmImage, int width, int height, char *out_filename, int format )
{
    int row, col;
    int quarterWidth = width / 4;
    int quarterHeight = height / 4;
    int colour = 0;

    create_PBM_Image( pbmImage, width, height );

    // Determine if the image requested is height-long or width-long
    int isWide = width >= height;
    int strokeStart, strokeEnd, strokeLength;

    strokeStart = 0;

    // Construct the white rectangle making up 1/2 total width and 1/2 total height
    for ( row = 0; row < pbmImage->height; row++ )
    {
        for ( col = 0; col < pbmImage->width; col++ )
        {
            if ( row >= quarterHeight && row < (quarterHeight * 3) && col >= quarterWidth && col < (quarterWidth * 3) )
            {
                colour = 0;
            }
            else
            {
                colour = 1;
            }
            pbmImage->image[row][col] = colour;
        }
    }

    // If image is width-long
    if ( isWide )
    {
        // then the length of our line stroke is equal to width/height
        strokeLength = width / height;
        strokeEnd = strokeLength;

        // draw the line from one corner of the image to the other
        // while also drawing the opposite line simoultaneously
        for ( row = 0; row < pbmImage->height; row++ )
        {
            for ( col = 0; col < pbmImage->width; col++ )
            {
                if( col >= strokeStart && col < strokeEnd )
                {
                    pbmImage->image[row][col] = 1;
                    pbmImage->image[row][width - col - 1] = 1;
                }
            }
            strokeStart = strokeEnd;
            strokeEnd += strokeLength;
        }
    }
    else
    {
        // Else it is height-long and the stroke length is equal to height/width
        strokeLength = height / width;
        strokeEnd = strokeLength;

        // draw the line from one corner of the image to the other
        // while also drawing the opposite line simoultaneously
        for ( col = 0; col < pbmImage->width; col++ )
        {
            for ( row = 0; row < pbmImage->height; row++ )
            {
                if ( row >= strokeStart && row < strokeEnd )
                {
                    pbmImage->image[row][col] = 1;
                    pbmImage->image[height - row - 1][col] = 1;
                }
            }
            strokeStart = strokeEnd;
            strokeEnd += strokeLength;
        }
    }

    // Save image to disk and free memory
    save_PBM_Image( pbmImage, out_filename, format );
    free_PBM_Image( pbmImage );

}

/**
 * @brief      { generate_pgm }
 *
 * @param      pgmImage      The pgm image
 * @param[in]  width         The width
 * @param[in]  height        The height
 * @param      out_filename  The out filename
 * @param[in]  format        The format
 *
 * @return     { void }
 */

void generate_pgm( struct PGM_Image *pgmImage, int width, int height, char *out_filename, int format )
{
    int quarterWidth = width / 4;
    int quarterHeight = height / 4;
    int colour = 0;

    create_PGM_Image( pgmImage, width, height, MAX_GRAY );

    // Determine if the image requested is height-long or width-long
    int isWide = width >= height;

    // Construct the white rectangle making up 1/2 total width and 1/2 total height
    for ( int row = 0; row < pgmImage->height; row++ )
    {
        for ( int col = 0; col < pgmImage->width; col++ )
        {
            if ( row >= quarterHeight && row < (quarterHeight * 3) && col >= quarterWidth && col < (quarterWidth * 3) )
            {
                colour = MAX_GRAY;
            }
            else
            {
                colour = 0;
            }
            pgmImage->image[row][col] = colour;
        }
    }

    // If image is width-long
    if (isWide)
    {

        int vEdgeStart, vEdgeEnd, vEdgeLength;
        float vShade, vGradient;

        vEdgeStart = quarterWidth;                      // sliding start point of the gradient
        vEdgeLength = width / height;                   // length of the iterative boundary of the gradient
        vEdgeEnd = vEdgeStart + vEdgeLength;            // sliding end point of the gradient
        vShade = (float) MAX_GRAY;                      // the shade of the current row/column
        vGradient = (float) MAX_GRAY / quarterHeight;   // the amount by which the shade is incremented with each iteration

        // Create the gradually darkening top/bottom triangles
        for ( int row = quarterHeight; row < (quarterHeight * 2); row++ )
        {
            for ( int col = quarterWidth; col < (quarterWidth * 2); col++ )
            {
                if ( col >= vEdgeStart )
                {
                    pgmImage->image[row][width - col - 1] = vShade;
                    pgmImage->image[height - row - 1][width - col - 1] = vShade;
                    pgmImage->image[row][col] = vShade;
                    pgmImage->image[height - row - 1][col] = vShade;
                }
            }
            vShade -= vGradient;
            vEdgeStart = vEdgeEnd;
            vEdgeEnd += vEdgeLength;
        }

        float hEdgeStart, hEdgeEnd, fHeight, fWidth;
        float hShade, hGradient, hEdgeLength;

        fHeight = (float) height;                       // height to calculate the ratio between height/width
        fWidth = (float) width;                         // height to calculate the ratio between height/width
        hEdgeStart = quarterHeight;                     // sliding start point of the gradient
        hEdgeLength = fHeight / fWidth;                 // length of the iterative boundary of the gradient
        hEdgeEnd =  hEdgeStart + hEdgeLength;           // sliding end point of the gradient
        hShade = (float) MAX_GRAY;                      // the shade of the current row/column
        hGradient = (float) MAX_GRAY / quarterWidth;    // the amount by which the shade is incremented with each iteration

        // Create the gradually darkening left/right triangles by applying
        // the same transformation but transposed
        for ( int col = quarterWidth; col < (quarterWidth * 2); col++ )
        {
            for ( int row = quarterHeight; row < (quarterHeight * 2); row++ )
            {
                if ( row >= (int) hEdgeStart )
                {
                    pgmImage->image[row][width - col - 1] = hShade;
                    pgmImage->image[height - row - 1][width - col - 1] = hShade;
                    pgmImage->image[row][col] = hShade;
                    pgmImage->image[height - row - 1][col] = hShade;
                }
            }
            hShade -= hGradient;
            hEdgeStart = hEdgeEnd;
            hEdgeEnd += hEdgeLength;
        }
    }
    else
    {
        // Else if the image is width-long
        int vEdgeStart, vEdgeEnd, vEdgeLength;
        float vShade, vGradient;

        vEdgeStart = quarterHeight;                             // sliding start point of the gradient
        vEdgeLength = height / width;                           // length of the iterative boundary of the gradient
        vEdgeEnd = vEdgeStart + vEdgeLength;                    // sliding end point of the gradient
        vShade = (float) MAX_GRAY;                              // the shade of the current row/column
        vGradient = (float) MAX_GRAY / quarterWidth;            // the amount by which the shade is incremented with each iteration

        // Create the gradually darkening top/bottom triangles
        for ( int col = quarterWidth; col < (quarterWidth * 2); col++ )
        {
            for ( int row = quarterHeight; row < (quarterHeight * 2); row++ )
            {
                if ( row >= vEdgeStart )
                {
                    pgmImage->image[row][width - col - 1] = vShade;
                    pgmImage->image[height - row - 1][width - col - 1] = vShade;
                    pgmImage->image[row][col] = vShade;
                    pgmImage->image[height - row - 1][col] = vShade;
                }
            }
            vShade -= vGradient;
            vEdgeStart = vEdgeEnd;
            vEdgeEnd += vEdgeLength;
        }

        float hEdgeStart, hEdgeEnd, fHeight, fWidth;
        float hShade, hGradient, hEdgeLength;

        fHeight = (float) height;                           // height to calculate the ratio between height/width
        fWidth = (float) width;                             // height to calculate the ratio between height/width
        hEdgeStart = quarterWidth;                          // sliding start point of the gradient
        hEdgeLength = fWidth / fHeight;                     // length of the iterative boundary of the gradient
        hEdgeEnd =  hEdgeStart + hEdgeLength;               // sliding end point of the gradient
        hShade = (float) MAX_GRAY;                          // the shade of the current row/column
        hGradient = (float) MAX_GRAY / quarterHeight;       // the amount by which the shade is incremented with each iteration

        // Create the gradually darkening left/right triangles by applying
        // the same transformation but transposed
        for ( int row = quarterHeight; row < (quarterHeight * 2); row++ )
        {
            for ( int col = quarterWidth; col < (quarterWidth * 2); col++ )
            {
                if ( col >= (int) hEdgeStart )
                {
                    pgmImage->image[row][width - col - 1] = hShade;
                    pgmImage->image[height - row - 1][width - col - 1] = hShade;
                    pgmImage->image[row][col] = hShade;
                    pgmImage->image[height - row - 1][col] = hShade;
                }
            }
            hShade -= hGradient;
            hEdgeStart = hEdgeEnd;
            hEdgeEnd += hEdgeLength;
        }
    }

    save_PGM_Image( pgmImage, out_filename, format );
    free_PGM_Image( pgmImage );

}

/**
 * @brief      { generate_ppm }
 *
 * @param      ppmImage      The ppm image
 * @param[in]  width         The width
 * @param[in]  height        The height
 * @param      out_filename  The out filename
 * @param[in]  format        The format
 *
 * @return     { void }
 */

void generate_ppm( struct PPM_Image *ppmImage, int width, int height, char *out_filename, int format )
{

    create_PPM_Image( ppmImage, width, height, MAX_GRAY );

    // Useful dimension values
    int thirdWidth = width / 3;
    int halfWidth = width / 2;
    int halfHeight = height / 2;

    // The amount by which the shades are incremented/decremented with each iteration
    float gradient = (float) MAX_GRAY / halfHeight;

    // initialize component shades
    float rShade = 0;
    float gShade = MAX_GRAY;
    float bShade = 0;

    float upShade = 0;
    float downShade = MAX_GRAY;


    // Colour Gradients on Upper Half
    for ( int row = 0; row < halfHeight; row++ )
    {
        for ( int col = 0; col < thirdWidth; col++ )
        {

            // red gradient
            ppmImage->image[row][col][0] = MAX_GRAY;
            ppmImage->image[row][col][1] = rShade;
            ppmImage->image[row][col][2] = rShade;

            // green gradient
            ppmImage->image[row][col + thirdWidth][0] = gShade;
            ppmImage->image[row][col + thirdWidth][1] = MAX_GRAY;
            ppmImage->image[row][col + thirdWidth][2] = gShade;

            // blue gradient
            ppmImage->image[row][col + (thirdWidth * 2)][0] = bShade;
            ppmImage->image[row][col + (thirdWidth * 2)][1] = bShade;
            ppmImage->image[row][col + (thirdWidth * 2)][2] = MAX_GRAY;

        }
        rShade += gradient;
        gShade -= gradient;
        bShade += gradient;
    }

    // Gray Gradients on Lower Half
    for ( int row = halfHeight; row < height; row++ )
    {
        for ( int col = 0; col < halfWidth; col++ )
        {

            // black to white, top to bottom
            ppmImage->image[row][col][0] = upShade;
            ppmImage->image[row][col][1] = upShade;
            ppmImage->image[row][col][2] = upShade;

            // white to black, top to bottom
            ppmImage->image[row][col + halfWidth][0] = downShade;
            ppmImage->image[row][col + halfWidth][1] = downShade;
            ppmImage->image[row][col + halfWidth][2] = downShade;

        }
        upShade += gradient;
        downShade -= gradient;
    }

    struct PGM_Image pgmImageRed, pgmImageGreen, pgmImageBlue;

    char pgm_red_filename[100];
    strcpy(pgm_red_filename, "Red_PGM_Copy_From_");
    strcat(pgm_red_filename, out_filename);

    char pgm_green_filename[100];
    strcpy(pgm_green_filename, "Green_PGM_Copy_From_");
    strcat(pgm_green_filename, out_filename);

    char pgm_blue_filename[100];
    strcpy(pgm_blue_filename, "Blue_PGM_Copy_From_");
    strcat(pgm_blue_filename, out_filename);

    create_PGM_Image( &pgmImageRed, width, height, MAX_GRAY );
    create_PGM_Image( &pgmImageGreen, width, height, MAX_GRAY );
    create_PGM_Image( &pgmImageBlue, width, height, MAX_GRAY );

    copy_PPM_to_PGM( ppmImage, &pgmImageRed, 0);
    copy_PPM_to_PGM( ppmImage, &pgmImageGreen, 1);
    copy_PPM_to_PGM( ppmImage, &pgmImageBlue, 2);

    save_PGM_Image( &pgmImageRed, pgm_red_filename, format);
    save_PGM_Image( &pgmImageGreen, pgm_green_filename, format);
    save_PGM_Image( &pgmImageBlue, pgm_blue_filename, format);
    save_PPM_Image( ppmImage, out_filename, format );

    free_PPM_Image( ppmImage );
    free_PGM_Image( &pgmImageRed );
    free_PGM_Image( &pgmImageGreen );
    free_PGM_Image( &pgmImageBlue );

}

/**
 * @brief      { main }
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 *
 * @return     { always returns integer 0 so as to not cause Makefile to fail prematurely }
 */

int main( int argc, char **argv )
{

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

    // check input against validation rules and exit if it fails
    int e = check_args(type, width, height, out_filename, format);
    if (e)
    {
        exit(0);
    }

    // call the appropriate function for the requested image type
    switch(type)
    {
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

    return 0;

}

