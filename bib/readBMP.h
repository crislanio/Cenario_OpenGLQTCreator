/*
 *  readBMP.h
 *
 *  Created by Nina Amenta on Sun May 23 2004.
 *  Free to good home!
 *
 */


/* Image type - contains height, width, and data */
struct BMPImage {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct BMPImage BMPImage;

/* Function that reads in the image; first param is filename, second is image struct */
/* As side effect, sets w and h */
int BMPImageLoad(char* filename, BMPImage* image);