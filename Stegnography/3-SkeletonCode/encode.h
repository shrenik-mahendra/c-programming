#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
*/

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;					//store name of src_image -> argv[2]
    FILE *fptr_src_image;					//open src_image in 'r' mode
    uint image_capacity;					//store size of src_image
    char image_data[MAX_IMAGE_BUF_SIZE];	// array -> image_data[8]: to fetch 8 bytes from src_image

    /* Secret File Info */
    char *secret_fname;							//store name of secret file -> argv[3]
    FILE *fptr_secret;							//open secret file in 'r' mode
    char extn_secret_file[MAX_FILE_SUFFIX];		//array to store ".txt" -> extn_secret_file[4]
    char secret_data[MAX_SECRET_BUF_SIZE];		//array -> secret_data[1]: to fetch 1 byte from secret file
    long size_secret_file;						//store secret file size

    /* Dest Image Info */
    char *stego_image_fname;					//store name of dest_image if given -> argv[4] else, store "stego.bmp"
    FILE *fptr_stego_image;						//open dest_image in 'w' mode
} EncodeInfo;										// EncodeInfo -> typedef name of structure


/* Encoding function prototype -- definitions written in encode.c */

/* Check operation type */
OperationType check_operation_type(char *argv[]);			

/* To check for no. of arguments passed */
Status check_encode_files(int argc);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);	

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);					

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);				

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image);		

/* Encode Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* encoding size of extn and file */
Status encode_size( int size, FILE *fptr_src_image, FILE *fptr_stego_image );

/* encode extension size to lsb */
Status encode_size_to_lsb( int size, char *buffer );

/* Encode secret file extenstion */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif


