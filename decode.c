/* Description:
 * This decode.c file contains all functions required to decode hidden data from a
 * BMP stego image using LSB steganography. It validates inputs, extracts
 * the magic string, file extension, file size, and finally retrieves the
 * original secret file data by reading LSB bits from the image.
 */

#include <stdio.h>
#include<string.h>
#include<ctype.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include<stdlib.h>
/* Function Definitions */

//validate the file extensions
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo){
    //validate .bmp file first
    char *check1;
    check1=strstr(argv[2],".bmp");
    if(check1==NULL || *(check1 + strlen(".bmp")) !='\0'){
        printf("ERROR:\t%s file not supported!!\n",argv[2]);
        return e_failure;
    }
    decInfo->dec_stego_image_fname=argv[2];

    //validate output file
    //i.e check is it provided or not
    if(argv[3]!=NULL){
        char str[70];
        strcpy(str,argv[3]);
        //if provided,only store file name i.e upto '.'
        char *token = strtok(str,".");
        decInfo->output_file_fname = malloc(50);
        strcpy(decInfo->output_file_fname , token);
    }
    else{
        //if not provided, then create by default
        printf("INFO:\toutput file not mentioned. Creating decode_msg as default...\n");
        decInfo->output_file_fname = malloc(50);
        strcpy(decInfo->output_file_fname , "decode_msg");
    }
    // No failure return e_success
    return e_success;    
}

//function definiton to perform decoding
Status do_decoding(DecodeInfo *decInfo){
    //calling open files function
    printf("INFO:\tOpening required files\n");
    if(open_dec_files(decInfo)==e_success){
        printf("INFO:\tOpened %s\n",decInfo->dec_stego_image_fname);
        printf("INFO:\tDone\n");
    }
    else{
        return e_failure;                                
    }
    //first skip the header of 54 bytes
    printf("INFO:\tSkipping the header\n");
    fseek(decInfo->fptr_dec_stego_image,54,SEEK_SET);

    printf("INFO:\t---Decoding procedure started---\n");
    
    //function call to decode magic string
    printf("INFO:\tDecoding Magic String Signature\n");
    //check if image is encoded or not by searching the magic string
    if(decode_magic_string(decInfo)==e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to decode magic string\n");
        printf("INFO:\t%s file is not encoded\n",decInfo->dec_stego_image_fname);
        return e_failure;
    }

    //function call to decode secret file extension size
    printf("INFO:\tDecoding secret File extension size\n");
    if(decode_secret_file_extn_size(decInfo) == e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to decode secret file extension size\n");
        return e_failure;
    }

    //function call to decode secret file extension
    printf("INFO:\tDecoding secret File Extension\n");
    if(decode_secret_file_extn(decInfo)==e_success){
        printf("INFO:\tDone\n");

        //open the output file after decoding the extension and concat it to the file name
        printf("INFO:\tOpening %s\n",decInfo->output_file_fname);
        decInfo->fptr_dec_output_file = fopen(decInfo->output_file_fname, "w");
        // Do Error handling
        if (decInfo->fptr_dec_output_file == NULL)
        {
    	    perror("fopen");
    	    fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_file_fname);

    	    return e_failure;
        }
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to decode secret file extension\n");
        return e_failure;
    }
    
    
    //function call to decode secret file size
    printf("INFO:\tDecoding secret File Size\n");
    if(decode_secret_file_size(decInfo) == e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to decode secret file size\n");
        return e_failure;
    }
    
    //function call to decode secret file data
    printf("INFO:\tDecoding secret File Data\n");
    if(decode_secret_file_data(decInfo) == e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to decode secret file data\n");
        return e_failure;
    }
    //no errors means
    return e_success;
}

//function definiton to open src file
Status open_dec_files(DecodeInfo *decInfo){
    // Stego Image file
    decInfo->fptr_dec_stego_image = fopen(decInfo->dec_stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_dec_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dec_stego_image_fname);

    	return e_failure;
    }
    
    // No failure return e_success
    return e_success;
}

//to decode magic string
Status decode_magic_string(DecodeInfo *decInfo){
    char image_buffer[8];
    //take 8 bytes from src stego image
    int m_size = strlen(MAGIC_STRING);
    int i;
    for(i=0;i<m_size;i++){
        if(fread(image_buffer,8,1,decInfo->fptr_dec_stego_image)!=1)
            return e_failure;
        //call to extract lsb's and store one-one character in char array
        decInfo->magic_str[i]=decode_byte_from_lsb(image_buffer);
    }
    decInfo->magic_str[i]='\0';
    //compare the magic string decoded from image with original magic string
    if(strcmp(MAGIC_STRING,decInfo->magic_str)==0){
        printf("INFO:\tMagic strings are matching\n");
        return e_success;
    }
    else{
        //printf("ERROR:\tMagic strings are not matching\n");
        return e_failure;
    }
}

//decode size of secret file extension
Status decode_secret_file_extn_size(DecodeInfo *decInfo){
    char image_buffer[32];
    //take 32 bytes from src stego image
    if(fread(image_buffer,32,1,decInfo->fptr_dec_stego_image)!=1)
        return e_failure;
    //call to extract lsb's and store size in struct member
    decInfo->size_of_extn = decode_size_from_lsb(image_buffer);
    
    return e_success;
}

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo){
    char image_buffer[8];
    char str[decInfo->size_of_extn];
    //take 8 bytes from src stego image
    int i;
    for(i=0;i<decInfo->size_of_extn;i++){
        if(fread(image_buffer,8,1,decInfo->fptr_dec_stego_image)!=1)
            return e_failure;
        //call to extract lsb's
        str[i]=decode_byte_from_lsb(image_buffer);
    }
    str[decInfo->size_of_extn]='\0';
    //after decoding the extension, concatenate it with output file name
    strcat(decInfo->output_file_fname, str);

    //printf("Info:\t%s\n",str);

    
    return e_success;
}

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo){
    char image_buffer[32];
    //take 32 bytes from src stego image
    if(fread(image_buffer,32,1,decInfo->fptr_dec_stego_image)!=1)
        return e_failure;
    //call to extract lsb's and store size in struct member
    decInfo->size_secret_file = decode_size_from_lsb(image_buffer);
    printf("INFO:\t%ld\n", decInfo->size_secret_file);
    return e_success;
}

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo){
    fseek(decInfo->fptr_dec_output_file,0,SEEK_SET);
    char image_buffer[8];
    //take 8 bytes from src stego image
    int i;
    char ch;
    for(i=0; i<decInfo->size_secret_file; i++){
        if(fread(image_buffer,8,1,decInfo->fptr_dec_stego_image)!=1)
            return e_failure;
        //call to extract lsb's from image
        ch=decode_byte_from_lsb(image_buffer);
        fputc(ch,decInfo->fptr_dec_output_file);
    }
    return e_success;

}

//to decode lsb bytes
char decode_byte_from_lsb(char *image_buffer){
    char ch=0;
    for(int i=0; i<8;i++){
        //extract lsb's from data and form a character
        char get = image_buffer[i] & 1;
        ch = ch | (get<<i);
    }
    return ch;
}

/* decode a bytes from LSB of image data array */
int decode_size_from_lsb(char *image_buffer){
    int size=0;
    for(int i=0;i<32;i++){
        //extract lsb's from data and form a integer
        int get = image_buffer[i] & 1;
        size = size | (get<<i);
    }

    return size;
}