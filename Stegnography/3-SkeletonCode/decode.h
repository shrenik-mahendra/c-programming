#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
	 /* Stego Image Info */
    char *stego_image_fname;					//store name of encoded image
    FILE *fptr_stego_image;						//file pointer to encoded image
	
    /* Secret File Info */
    char *secret_fname;							//store name of secret file
    FILE *fptr_secret;							//file pointer to secret file
    
  	int size_file_extn;							//store size of secret file extension
	char extn_file[MAX_FILE_SUFFIX];			//exten_file[4] - store ".txt"
	long size_secret_file;						//store the size of secret file
	char secret_data[MAX_SECRET_BUF_SIZE];			//secret_data[1]- fetch 1B 
	
} DecodeInfo;

/* Decoding function prototype */

/* To check for no. of arguments passed */
Status check_decode_files(int argc);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform decoding operations */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_for_decoding(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string( char *magic_string, DecodeInfo *decInfo);

/* Get LSB bits from image bytes and form a byte */
Status get_byte_from_lsb(char data, FILE *fptr_stego_image);

/* Decode secret file extenstion */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real encoding */
Status decode_data_to_file(FILE *fptr_stego_image, FILE *fptr_secret);

/* Decoding  extention size */
Status decode_size(int size, FILE *fptr_stego_image);

/* Decode size to lsb */
Status decode_size_to_LSB(int size, char *buffer);

/* Decoding file size */
long decode_file_size(FILE *fptr_stego_image);

#endif

