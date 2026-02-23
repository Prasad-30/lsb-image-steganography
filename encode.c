/* Description:
 * This encode.c file implements all functions required to encode secret data into a
 * BMP image using LSB steganography. It validates inputs, checks image
 * capacity, copies the BMP header, and embeds the magic string, file
 * extension, file size, and file data by modifying the least significant
 * bits of the image. The result is written into a new stego image.
 */

#include <stdio.h>
#include<string.h>
#include<ctype.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* Get secret file size
 * Input: secret file ptr
 * Output: size of secret file
 */
uint get_file_size(FILE *fptr_secret){
    fseek(fptr_secret,0,SEEK_END);
    long size= ftell(fptr_secret);
    rewind(fptr_secret);
    return size;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

//check type of opeartion i.e is it encoding or decoding
OperationType check_operation_type(char *argv[]){
    //return the enum values according the operations
    if(strcmp(argv[1],"-e")==0){
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0){
        return e_decode;
    }
    else{
        return e_unsupported;
    }
}

//validate the file extensions
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo){
    //validate .bmp file first
    char *check1;
    check1=strstr(argv[2],".bmp");
    //checking that .bmp is present or not and it should be at the end
    if(check1==NULL || *(check1 + strlen(".bmp")) !='\0'){
        printf("ERROR:\t%s file not supported!!\n",argv[2]);
        return e_failure;
    }
    //store valid argument in structure
    encInfo->src_image_fname=argv[2];

    //validate .txt file
    char *check2;
    check2=strstr(argv[3],".txt");
    //checking that .txt is present or not and it should be at the end
    if(check2==NULL || *(check2 + strlen(".txt")) !='\0'){
        printf("ERROR:\t%s file not supported!!\n",argv[3]);
        return e_failure;
    }
    //store valid argument in structure
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file,".txt");

    //first check that stego image file i.e dest image is present or not
    if(argv[4]!=NULL){
        char *check3;
        check3=strstr(argv[4],".bmp");
        //checking that .bmp is present or not and it should be at the end
        if(check3==NULL || *(check3 + strlen(".bmp")) !='\0'){
            printf("ERROR:\t%s file not supported!!\n",argv[4]);
            return e_failure;
        }
        //store valid argument in structure
        encInfo->stego_image_fname = argv[4];
    }
    else{
        //store default file name if it is not provided
        printf("INFO:\toutput file not mentioned. Creating stego.bmp as default...\n");
        encInfo->stego_image_fname = "stego.bmp";
    }

    // No failure return e_success
    return e_success;    
}

//do encoding function definition
Status do_encoding(EncodeInfo *encInfo){
    //calling open files function
    printf("INFO:\tOpening required files\n");
    if(open_files(encInfo)==e_success){
        printf("INFO:\tOpened %s\n",encInfo->src_image_fname);
        printf("INFO:\tOpened %s\n",encInfo->secret_fname);
        printf("INFO:\tOpened %s\n",encInfo->stego_image_fname);
        printf("INFO:\tDone\n");
    }
    else{
        return e_failure;                                       
    }
    
    //calling check_capacity function
    printf("INFO:\t---Encoding procedure started---\n");
    if(check_capacity(encInfo)==e_success){
        printf("INFO:\tDone. Found OK\n");
    }
    else{
        return e_failure;
    }

    //calling funtion to copy the bmp header
    printf("INFO:\tCopying Image Header\n");
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to copy\n");
        return e_failure;
    }
    //function call to encode magic string
    printf("INFO:\tEncoding Magic String Signature\n");
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to encode magic string\n");
        return e_failure;
    }   
    
    //function call to encode secret file extension size
    printf("INFO:\tEncoding %s File extension size\n",encInfo->secret_fname);
    if(encode_secret_file_extn_size(4, encInfo) == e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to encode secret file extension size\n");
        return e_failure;
    }

    //function call to encode secret file extension
    printf("INFO:\tEncoding %s File Extension\n",encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)==e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to encode secret file extension\n");
        return e_failure;
    }

    //function call to encode secret file size
    printf("INFO:\tEncoding %s File Size\n",encInfo->secret_fname);
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to encode secret file size\n");
        return e_failure;
    }
    
    
    //function call to encode secret file data
    printf("INFO:\tEncoding %s File data\n",encInfo->secret_fname);
    if(encode_secret_file_data(encInfo) == e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to encode secret file data\n");
        return e_failure;
    }

    //function call to copy left over data
    printf("INFO:\tCopying Leftover Data\n");
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success){
        printf("INFO:\tDone\n");
    }
    else{
        printf("ERROR:\tFailed to copy leftover data\n");
        return e_failure;
    }

    // CLOSE ALL FILE POINTERS HERE
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    //no errors return success
    return e_success;
}

//checking capacity
Status check_capacity(EncodeInfo *encInfo){
    //get source image file size by calling a function
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    
    //get secret file size and store it in a struct member
    printf("INFO:\tChecking for %s size\n",encInfo->secret_fname);
    //store it
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    //check .txt file is empty or not
    if(encInfo->size_secret_file > 0){
        printf("INFO:\tDone. Not Empty\n");
    }
    else{
        printf("ERROR:\t%s file is empty\n",encInfo->secret_fname);
        return e_failure;
    }
    //check the capacity of image file to store secret data
    printf("INFO:\tChecking for %s capacity to handle %s\n",encInfo->src_image_fname, encInfo->secret_fname);
    if(encInfo->image_capacity > 16+32+32+32+(encInfo->size_secret_file*8)){
        return e_success;
    }
    else{
        printf("ERROR:\t%s file capacity is not enough\n",encInfo->src_image_fname);
        return e_failure;
    }

}

//copying image header
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
    //declare array to store header
    char arr[54];
    //set the file offset at the begining of a source file
    fseek(fptr_src_image,0,SEEK_SET);
    //read first 54 bytes from source image i.e header
    if(fread(arr, 54, 1, fptr_src_image) != 1) //error checking i.e fread should return 1, if 1 block of 54 bytes read successfully
        return e_failure;

    //write 54 bytes from source image i.e header into dest image
    if(fwrite(arr, 54, 1, fptr_dest_image) != 1)
        return e_failure;

    return e_success;
}

//encode magic string function definiton
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){
    int magic_size=strlen(MAGIC_STRING);
    //call encode data to image function
    if(encode_data_to_image(MAGIC_STRING, magic_size, encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success){
        return e_success;
    }
    else{
        return e_failure;
    }
}

//encode secret file extension function definition
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){
    int extension_size=strlen(encInfo->extn_secret_file);
    //call encode data to image function
    if(encode_data_to_image(encInfo->extn_secret_file, extension_size, encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success){
        return e_success;
    }
    else{
        return e_failure;
    }
}

//encode secret file size definition
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){
    char image_buffer[32];
    //read 32 bytes directly from src image
    if(fread(image_buffer,32,1,encInfo->fptr_src_image)!=1)
        return e_failure;
    
    //encode 32 bits
    for(int i=0 ; i<32 ;i++){
        int get = (file_size>>i) & 1;
        image_buffer[i] = ( image_buffer[i] & (~1) ) | get;
    }
    //write 32 encoded bytes into stego image
    if(fwrite(image_buffer,32,1,encInfo->fptr_stego_image)!=1){
        return e_failure;
    }

    return e_success;

}

//encode size of secret file extension
Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo){
    char image_buffer[32];
    //read 32 bytes directly from src image
    if(fread(image_buffer,32,1,encInfo->fptr_src_image)!=1)
        return e_failure;
    
    //encode 32 bits 
    for(int i=0 ; i<32 ;i++){
        int get = (extn_size>>i) & 1;
        image_buffer[i] = ( image_buffer[i] & (~1) ) | get;
    }
    //write 32 encoded bytes into stego image
    if(fwrite(image_buffer,32,1,encInfo->fptr_stego_image)!=1){
        return e_failure;
    }

    return e_success;
}

//encode secret file data function definition
Status encode_secret_file_data(EncodeInfo *encInfo){
    char image_buffer[8];
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    for(int i=0; i<encInfo->size_secret_file;i++){
        //read 1-1 byte from secret file and store in secret_data array
        if(fread(encInfo->secret_data,1,1,encInfo->fptr_secret)!=1)
            return e_failure;
        //read 8 bytes from src image and store them in image buffer
        if(fread(image_buffer,8,1,encInfo->fptr_src_image)!=1)
            return e_failure;
        //function call to encode byte to lsb
        encode_byte_to_lsb(encInfo->secret_data[0],image_buffer);
        //write encoded 8 bytes into setgo image 
        if(fwrite(image_buffer,8,1,encInfo->fptr_stego_image)!=1)
            return e_failure;
    }
    return e_success;
}

//encode data to image function definition
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image){
    char image_buffer[8];
    for(int i=0;i<size;i++){
        //read 8 bytes from src image and store them in image buffer
        if(fread(image_buffer,8,1,fptr_src_image)!=1)
            return e_failure;
        //function call to encode byte to lsb
        encode_byte_to_lsb(data[i],image_buffer);
        //write encoded 8 bytes into setgo image 
        if(fwrite(image_buffer,8,1,fptr_stego_image)!=1)
            return e_failure;
    }
    return e_success;
}

//encode byte to lsb function defintion
Status encode_byte_to_lsb(char data, char *image_buffer){
    if(image_buffer==NULL){
        return e_failure;
    }
    for(int i=0;i<8;i++){
        //first get one-one byte from lsb of data i.e magic string
        int get = (data>>i) & 1;
        //then clear one-one bit from lsb of image buffer and set that bit with getted bit
        image_buffer[i] = ( image_buffer[i] & (~1) ) | get;
    }
    return e_success;
}

//copy left over data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
    char ch;
    //read one one byte from src image and write in a dest image
    while(fread(&ch,1,1,fptr_src)==1){
        fwrite(&ch,1,1,fptr_dest);
    }

    return e_success;
}