/*Student Name: Prasad Kumbhar
  Project Name: Steganography
  Date: 05-12-2025
  Description: Steganography is the technique of hiding secret data inside another file (called the cover file)
               in such a way that the presence of the hidden information is not detectable. 
               This project implements Image Steganography using the LSB (Least Significant Bit) method in a BMP image.
               A secret text file (.txt) is hidden inside BMP image, and later extracted back without altering the visual appearance of the image.
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include<stdlib.h>

int main(int argc, char *argv[])
{   
    //first check argument count for opeartion
    if(argc<2){
       //if arguments are not enough, terminate the program
       printf("\033[1;31mERROR:\tNo operation type provided\033[0m\n");
       printf("Usage:\n");
       printf("For Encode : %s -e <input.bmp> <secret.txt> <output.bmp>(optional)\n", argv[0]);
       printf("For Decode : %s -d <input.bmp> <output.txt>(optional)\n",argv[0]);
       return 0;
    }
    //now initially check what is the operation type
    if(check_operation_type(argv) == e_encode){
        //if it is encoding i.e -e, then check if it's having a sufficient argc count or not
        if(argc<4){
            //terminate the program if argc is not atleast 4
            printf("\033[1;31mERROR:\tProvide Sufficient arguments for Encoding\033[0m\n");
            printf("For Encode : %s -e <input.bmp> <secret.txt> <output.bmp>(optional)\n", argv[0]);

            return 0;
        }
        //create structure variable for encoding that stores information
        EncodeInfo encInfo;
        printf("INFO:\t---Selected encoding---\n");
        //call to validate the cmd line arguments
        if(read_and_validate_encode_args(argv, &encInfo) == e_success){
            printf("INFO:\t---read and validate arguments done---\n");
            //call to perform actual encoding
            if(do_encoding(&encInfo)== e_success){
                printf("------------------------------------------------\n");
                printf("INFO:\tEncoding operation is done successfully!\n");
                printf("INFO:\t%s file is created\n",encInfo.stego_image_fname);
                printf("------------------------------------------------\n");
            }
            else{
                return 0;
            }
        }
        else
        {
            return 0;
        }
        
    }
    //if the operation type is decoding -->
    else if(check_operation_type(argv) == e_decode){
        //if it is decoding i.e -d, then check if it's having a sufficient argc count or not
        if(argc<3){
            printf("\033[1;31mERROR:\tProvide Sufficient arguments for Decoding\033[0m\n");
            printf("For Decode : %s -d <input.bmp> <output.txt>(optional)\n",argv[0]);
            return 0;
        }
        //now create structure variable for decoding
        DecodeInfo decInfo;
        printf("INFO:\t---Selected decoding---\n");
        //read and validate the cmd arguments i.e if they are valid store them in struct member
        if(read_and_validate_decode_args(argv, &decInfo) == e_success){
            printf("INFO:\t---read and validate arguments done---\n");
            //function call to perform decoding
            if(do_decoding(&decInfo)== e_success){
                printf("------------------------------------------------\n");
                printf("INFO:\tDecoding operation is done successfully!\n");
                printf("INFO:\t%s file is created\n",decInfo.output_file_fname);
                printf("------------------------------------------------\n");
                //free the memory and close the file pointers
                free(decInfo.output_file_fname);
                fclose(decInfo.fptr_dec_output_file);
                fclose(decInfo.fptr_dec_stego_image);
            }
            else{
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    //if user give invalid operation type then print error message and also print Usage prompt
    else if(check_operation_type(argv) == e_unsupported){
        
        printf("\033[1;31mUnsupported operation type!!\033[0m\n");
        printf("Usage:\n");
        printf("For Encode : %s -e <input.bmp> <secret.txt> <output.bmp>(optional\n", argv[0]);
        printf("For Decode : %s -d <input.bmp> <output.txt>(optional)\n",argv[0]);
    }

    return 0;
}
