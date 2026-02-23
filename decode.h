/* Description:
 * This decode.h header file contains structures and function prototypes for decoding hidden data
 * from a BMP image using LSB technique.
 */

#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */           

typedef struct _DecodeInfo
{   
    char magic_str[5];
    /* Secret File Info */
    char *output_file_fname;
    FILE *fptr_dec_output_file;
    int size_of_extn;
    long size_secret_file;

    /* Stego Image Info */
    char *dec_stego_image_fname;
    FILE *fptr_dec_stego_image;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_dec_files(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Encode secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode a byte into LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

/* Encode a byte into LSB of image data array */
int decode_size_from_lsb(char *image_buffer);

#endif
