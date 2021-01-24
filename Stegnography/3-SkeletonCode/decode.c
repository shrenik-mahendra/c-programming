#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h" 

/* Function Definitions */

/* Check number of arguments passed
 * Input: argument counter - argc
 * Description: check if no. of arguments passed is >= 3 and <= 4.
 * Return: e_failure or e_success
*/
Status check_decode_files(int argc)			
{	
	if( argc < 3 || argc > 4 ) 
		return e_failure;
	else 
		return e_success;
}

/* Read and validate Decode arguments from argv
 * Input: argument variable - array of strings, pointer to 'DecodeInfo' structure
 * Output: get the file names of src_image and dest_file
 * Description: check if 
 				1. argv[2] is a .bmp file
 				2. if argv[3] is passed check if it is .txt file
 * Return: e_failure or e_success
*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	char * str = NULL;
	str = argv[2] + strlen(argv[2]) - strlen(IMAGE_EXTN);						//str points to extn of stego_image
	if( strcmp(str, IMAGE_EXTN) == 0 )											//compare str with ".bmp"
	{	
		decInfo->stego_image_fname = (char *)malloc( strlen(argv[2]) + 1);		//store the name of stego_image
		strcpy(decInfo->stego_image_fname, argv[2]);	
		if( argv[3] != NULL )					//if secret_file name passed
		{	
			str = argv[3] + strlen(argv[3]) - strlen(FILE_EXTN);			//str points to extn of secret_file
			if( strcmp(str, FILE_EXTN) == 0 )								//compare str with ".txt"
			{		
				decInfo->secret_fname = (char *)malloc( strlen(argv[3]) + 1);
				strcpy(decInfo->secret_fname, argv[3]);									//store the name of secret_file
				return e_success;
			}
			else
				return e_failure;
		}
		else						//if secret_file name is not passed -> assign default name "decode.txt" 
		{
			decInfo->secret_fname = (char *)malloc( strlen(FILE_FNAME) + 1);
			strcpy(decInfo->secret_fname, FILE_FNAME);
			return e_success;
		}
	}
	else
		return e_failure;
}

/* Perform decoding 
 * Input: pointer to 'DecodeInfo' structure
 * Description: open all the files using respective file pointers and perform decoding operations
 * Return: e_failure or e_success
*/
Status do_decoding(DecodeInfo *decInfo)
{
	if( open_files_for_decoding(decInfo) == e_success )				//open encoded_image, decode_file
	{
		printf("INFO: File '%s' opened in 'r' mode\n", decInfo->stego_image_fname);
		printf("INFO: File '%s' opened in 'w' mode\n", decInfo->secret_fname);
		fseek(decInfo->fptr_stego_image, HEAD_SIZE, SEEK_SET);						//move across the image Header to B54
		if( decode_magic_string(MAGIC_STRING, decInfo) == e_success )			//decode magic string
		{
			printf("INFO: Decoding Magic String successful\n");
			if( decode_secret_file_extn_size(decInfo) == e_success )					//decode secret file extension size		
			{
				printf("INFO: Secret file extension size decoded successfully\n");
				if( decode_secret_file_extn(FILE_EXTN, decInfo) == e_success )				//decode secret file extension
				{
					printf("INFO: Decoding secret file extension successful\n");
					if( decode_secret_file_size(decInfo) == e_success )					//decode secret file size
					{
						printf("INFO: Decoding secret file size successful\n");	
						if( decode_secret_file_data(decInfo) == e_success )					//decode secret file data
						{
							printf("INFO: Decoding secret file data successful\n");
							printf("INFO: Decoded data written to '%s'\n", decInfo->secret_fname);
							return e_success;
						}
						else
						{	
							printf("ERROR: Decoding secret file data failure\n");
							return e_failure;
						}
					}
					else
					{
						printf("ERROR: Decoding secret file size failure\n");
						return e_failure;
					}
				}
				else
				{
					printf("ERROR: Decoding secret file extension failure\n");
					return e_failure;
				}
			}
			else
			{
				printf("ERROR: Decoding secret file extension size failure\n");
				return e_failure;
			}
		}
		else
		{
			printf("ERROR: Decoding magic string failure - '%s' is not encoded image\n", decInfo->stego_image_fname);
			return e_failure;
		}
	}
	else
		return e_failure;
}


/* Open all the files 
 * Input: pointer to 'DecodeInfo' structure
 * Output: Get File pointers for i/p and o/p files
 * Description: open files using respective file pointers  
 * Return: e_failure or e_success or errors
*/
Status open_files_for_decoding(DecodeInfo *decInfo)
{
	printf("INFO: Opening required files\n");									//open encoded_image -> 'r' mode
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if(decInfo->fptr_stego_image == NULL)
    {
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
    	return e_failure;
    }
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");						//open dest_secret_file -> 'w' mode
    if(decInfo->fptr_secret == NULL)
    {
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
    	return e_failure;
    }
    return e_success;
}

/* Decode Magic String
 * Input: magic string, pointer to 'DecodeInfo' structure
 * Output: Magic string bytes
 * Description: decode 16B bytes of encoded image and getback magic string  
 * Return: e_failure or e_success or errors
*/
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
	int i, flag = 0;
	for( i = 0; i < strlen(magic_string); i++ )			//run loop for strlen("#*") -> 2 times and pass # and * respectively
	{
		if( get_byte_from_lsb(magic_string[i], decInfo->fptr_stego_image) == e_success )
			flag = 1;
		else
			flag = 0;
	}
	if( flag == 1 ) 
		return e_success;
	else 
		return e_failure;	
}

/* Get byte from LSB bits
 * Input: 1Byte char data, file pointer to stego_image
 * Description:  Get LSB bits from image bytes and form a byte
 * Return: e_failure or e_success or errors
*/
Status get_byte_from_lsb(char data, FILE *fptr_stego_image)
{
	char image_buffer[8], byte = 0x00;
	int i;
	fread( image_buffer, sizeof(image_buffer), 1, fptr_stego_image );				//read 8 bytes at a time
	for( i = 0; i < sizeof(image_buffer); i++ )
	{
		byte = ( (image_buffer[i] & 0x01) << (sizeof(image_buffer)-i-1) ) | byte;  //fetch LSB from byte and form Byte
	}
	if( byte == data ) 
		return e_success;
	else 
		return e_failure;
}

/* Decode secret file extenstion 
 * Input: pointer to 'DecodeInfo' structure
 * Description: decode 32B bytes of encoded image and getback size of secret_file 
 * Return: e_failure or e_success or errors
*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	if( decode_size( strlen(FILE_EXTN), decInfo->fptr_stego_image) == e_success )
		return e_success;
	else
		return e_failure;
}

/* Encoding file extention size
 * Input: strlen(".txt"), file pointer to encoded image
 * Description: decode 32B bytes of encoded image and getback size of secret_file 
 * Return: e_failure or e_success or errors
*/
Status decode_size(int size, FILE *fptr_stego_image)
{
	char img_buff[sizeof(int)*8], lsb;
	int i, byte = 0x00000000;
	fread( img_buff, sizeof(img_buff), 1, fptr_stego_image );			//read 32 bytes from encoded image
	for( i = 0; i < sizeof(img_buff); i++ )
	{
		lsb = !!( (img_buff[i] & 0x01) );						//get LSB from 8 bytes
		byte = ( (int)lsb << (sizeof(img_buff)-i-1) ) | byte;		//form a byte from 8 bist
	}
	if( byte == size )			//compare with strlen(".txt")
		return e_success;
	else
		return e_failure;
}

/* Decode secret file extenstion 
 * Input: ".txt" extension, structure pointer to DecodeInfo
 * Description: decode 32B bytes of encoded image and getback extension of secret file 
 * Return: e_failure or e_success or errors
*/
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo)
{
	int i, flag = 0;
	for( i = 0; i < strlen(file_extn); i++ )
	{
		if( get_byte_from_lsb(file_extn[i], decInfo->fptr_stego_image) == e_success )
			flag = 1;
		else
			flag = 0;
	}
	if( flag == 1 ) 
		return e_success;
	else 
		return e_failure;
}

/* Decode secret file size 
 * Input: structure pointer to DecodeInfo
 * Description:  decode 32B of image and get the size of secret file
 * Return: e_failure or e_success or errors
*/
Status decode_secret_file_size( DecodeInfo *decInfo)
{
	decInfo->size_secret_file = decode_file_size( decInfo->fptr_stego_image );
	printf("INFO: Decoded secret file size = %ld\n", decInfo->size_secret_file);
	return e_success;
}

/* Decode file size 
 * Input: file pointer to image stego
 * Description:  decode 32B of image and get the size of secret file
 * Return: e_failure or e_success or errors
*/
long decode_file_size(FILE *fptr_stego_image)
{
	char img_buff[sizeof(int)*8], lsb;
	int i, byte = 0x00000000;
	fread( img_buff, sizeof(img_buff), 1, fptr_stego_image );			//read 32 bytes
	for( i = 0; i < sizeof(img_buff); i++ )
	{
		lsb = !!( (img_buff[i] & 0x01) );				//get LSB of bytes
		byte = ( (int)lsb << (sizeof(img_buff)-i-1) ) | byte;			//generate 4 bytes from bits
	}
	return byte;
}

/* Decode secret file data
 * Input: structure pointer to DecodeInfo
 * Description:  decode sizeof(file)*8 bytes of image and get the data of secret file
 * Return: e_failure or e_success or errors
*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	int i;	
	for( i = 0; i < decInfo->size_secret_file; i++ )
	{
		 decode_data_to_file(decInfo->fptr_stego_image, decInfo->fptr_secret);
	}
	return e_success;	
}

/* Decode function, which does the real decoding 
 * Input: file pointer to stego_image and secret_file
 * Description:  decode 8 bytes of image at a time and get the data of secret file
 * Return: e_failure or e_success or errors
*/
Status decode_data_to_file(FILE *fptr_stego_image, FILE *fptr_secret)
{
	int i;
	char img_buff[8], byte = 0x00;
	fread( img_buff, sizeof(img_buff), 1, fptr_stego_image );			//fetch 8 bytes at a time 
	for( i = 0; i < sizeof(img_buff); i++)
	{
		byte = ( (img_buff[i] & 0x01) << (sizeof(img_buff)-i-1) ) | byte;		//form byte from LSB bits
	}
	fputc(byte, fptr_secret);		//print the decode data to dest_file
	return e_success;
}

