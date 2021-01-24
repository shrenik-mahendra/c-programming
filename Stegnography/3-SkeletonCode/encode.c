#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h" 

/* Function Definitions */

/* Check operation type
 * Input: argument variable - array of strings
 * Description: check if argv[1] == -e or -d
 * Return: e_encode or e_decode or e_unsupported
*/
OperationType check_operation_type(char *argv[])
{	
	if( strcmp(argv[1], "-e") == 0 )
		return e_encode;
	else if( strcmp(argv[1], "-d") == 0)
		return e_decode;
	else
		return e_unsupported;	
}

/* Check number of arguments passed
 * Input: argument counter - argc
 * Description: check if no. of arguments passed is >= 4 and <= 5.
 * Return: e_failure or e_success
*/
Status check_encode_files(int argc)			
{	
	if( argc < 4 || argc > 5 ) 
		return e_failure;
	else 
		return e_success;
}

/* Read and validate Encode arguments from argv
 * Input: argument variable - array of strings, pointer to 'EncodeInfo' structure
 * Output: get the file names of src_image, secret file and dest_image
 * Description: check if 
 				1. argv[2] is a .bmp file
 				2. argv[3] is a .txt file
 				3. if argv[4] is passed check if it is .bmp file
 * Return: e_failure or e_success
*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	char * str = NULL;
	str = argv[2] + strlen(argv[2]) - strlen(IMAGE_EXTN);							//str points to the extension of src_image
	if( strcmp(str, IMAGE_EXTN) == 0 )												//compare str with ".bmp"
	{	
		encInfo->src_image_fname = (char *)malloc( strlen(argv[2]) + 1);			//store the name of src_image
		strcpy(encInfo->src_image_fname, argv[2]);										
		str = argv[3] + strlen(argv[3]) - strlen(FILE_EXTN);						//str points to the extension of secret file
		strcpy(encInfo->extn_secret_file, str);
		if( strcmp(str, FILE_EXTN) == 0 )											//compare str with ".txt"
		{	
			encInfo->secret_fname = (char *)malloc( strlen(argv[3]) + 1);			//store the name od secret_file
			strcpy(encInfo->secret_fname, argv[3]);			
			if( argv[4] != NULL )												//if dest_image name is passed
			{
				str = argv[4] + strlen(argv[4]) - strlen(IMAGE_EXTN);					//str points to the extension of dest_image
				if( strcmp(str, IMAGE_EXTN) == 0 )										//compare str with ".bmp"
				{		
					encInfo->stego_image_fname = (char *)malloc( strlen(argv[4]) + 1);
					strcpy(encInfo->stego_image_fname, argv[4]);						//store the name of dest_image
					return e_success;
				}
				else
					return e_failure;
			}
			else									//if dest_image name is not passes -> assign default name: "stego.bmp"
			{
				encInfo->stego_image_fname = (char *)malloc( strlen(IMAGE_FNAME) + 1);
				strcpy(encInfo->stego_image_fname, IMAGE_FNAME);
				return e_success;
			}
		}
		else
			return e_failure;
	}
	else
		return e_failure;
}

/* Perform the encoding 
 * Input: pointer to 'EncodeInfo' structure
 * Description: function tasks 
 				1. open all the files using respective file pointers
 				2. check if files are empty or not
 				3. check capacity to encode
 * Return: e_failure or e_success
*/
Status do_encoding(EncodeInfo *encInfo)
{	
	if( open_files(encInfo) == e_success )						//open src_image, secret_file, dest_file
	{	
		printf("INFO: File '%s' opened in 'r' mode\n", encInfo->src_image_fname);
		printf("INFO: File '%s' opened in 'r' mode\n", encInfo->secret_fname);
		printf("INFO: File '%s' opened in 'w' mode\n", encInfo->stego_image_fname);
		if( check_capacity(encInfo) == e_success )				//check if the src_image has capacity to encode secret_file
		{
			printf("INFO: Capacity check for encoding successful\n");
			if( copy_bmp_header( encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )	  //copy bmp image header
			{	
				printf("INFO: Copying .bmp header successful\n");
				if( encode_magic_string(MAGIC_STRING, encInfo) == e_success )	//encode magic string		
				{	
					printf("INFO: Encoding Magic String succesful\n");			//encode size of secret file extension
					if( encode_size( strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image ) == e_success)
					{
						printf("INFO: Encoding size of secret file extension successful\n");
						if( encode_secret_file_extn(encInfo->extn_secret_file, encInfo ) == e_success)		//encode secret file extension
						{
							printf("INFO: Encoding secret file extension successful\n");
							if( encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success )		//encode secret file size
							{
								printf("INFO: Encoding secret file size is successful\n");
								if( encode_secret_file_data(encInfo) == e_success )
								{
									printf("INFO: Encoding secret file data successful\n");				//encode secret file data
									if( copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )
									{	printf("INFO: Coping remaing bytes from src_image completed\n");	//copy remaining bytes unmodified
										return e_success;
									}
									else
									{
										return e_failure;
									}
								}
								else
								{
									printf("ERROR: Encoding secret file data failure\n");
									return e_failure;	
								}
							}
							else
							{	printf("ERROR: Encoding secret file size is failure\n");
								return e_failure;
							}
						}
						else
						{	
							printf("ERROR: Encoding secret file extension failure\n");
							return e_failure;
						}				
					}
					else
					{	printf("ERROR: Encoding size of secret file extension failure\n");
						return e_failure;
					}
				}
				else
				{
					printf("ERROR: Encoding Magic String failure\n");
					return e_failure;
				}
			}	
			else
			{	
				printf("ERROR: Copying .bmp header unsuccessful\n");
				return e_failure;
			}
		}
		else
			return e_failure;
	}
	else
		return e_failure;	
}


/* Open all the files 
 * Input: pointer to 'EncodeInfo' structure
 * Output: Get File pointers for i/p and o/p files
 * Description: open files using respective file pointers  
 * Return: e_failure or e_success or errors
*/
Status open_files(EncodeInfo *encInfo)
{
	printf("INFO: Opening required files\n");
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");				//open src_image -> 'r' mode
    if (encInfo->fptr_src_image == NULL)
    {
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");					//open secret file -> 'r' mode
    if (encInfo->fptr_secret == NULL)
    {
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");				//open dest_image -> 'w' mode
    if (encInfo->fptr_stego_image == NULL)
    {
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }
    return e_success;
}

/* Check encoding capacity of image 
 * Input: pointer to 'EncodeInfo' structure
 * Output: Get size of image and file and capacity 
 * Description: get size of image and file to check capacity of image to encode file data
 * Return: e_failure or e_success or errors		
*/
Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);									//size of secret_file
	printf("INFO: Size of '%s' - %ld bytes\n", encInfo->secret_fname, encInfo->size_secret_file);
	if( encInfo->size_secret_file == 0 )															//check if secret file is empty or not
	{	printf("ERROR: Secret file is empty - add content\n");
		return e_failure;
	}
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);						//size of src_image
	printf("INFO: Size of '%s' - %d bytes\n", encInfo->src_image_fname, encInfo->image_capacity);
	if( encInfo->image_capacity == 0 )														//check if src_image is empty or not				
	{	printf("ERROR: Imprefect src_image\n");
		return e_failure;
	}	
	/* check for image capacity */
	/* 	54B -> size of bmp image header
		16B -> encoding magic string "#*" -> strlen("#*") = 2B -> 16bits
		32B -> encoding size of secret_file extension: .txt => size = 4 ( integer val - sizeof(int) = 4B = 32bits )
		32B -> encoding secret_file extension: ".txt" -> strlen = 4B -> 32bits
		32B -> encoding size of secret_file: assume, size = 16 (int val. - sizeof(int) = 4B = 32bits )
		16*8 = 128B -> encoding secret file data: 16B - 128bits
	*/																											
	if( encInfo->image_capacity >= 54 + 16 + 32 + 32 + 32 + encInfo->size_secret_file * 8 )			
		return e_success;
	else
	{	printf("ERROR: '%s' has no capacity for encoding\n", encInfo->src_image_fname);
		return e_failure;
	}	
}
				
/* Get secret_file size
 * Input: file pointer to secret_file
 * Description: find the size of the file
 * Return type: size of secret_file - position of file pointer at end
*/
uint get_file_size(FILE *fptr)
{	
	fseek(fptr, 0L, SEEK_END);
	return ftell(fptr);
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18 and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);
    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    // Return image capacity
    return width * height * 3;
}

/* Copy bmp image header
 * Input: src_image and dest_image pointer
 * Output: 54 B header copied from src_image to dest_image
 * Description: 1st 54B (0-53) is copied w/o modification
 * Return: e_failure or e_success 
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char ch;
	rewind(fptr_src_image);
	while( ftell(fptr_src_image) != 54 )			//copy bytes: B0 - B53
	{	ch = fgetc(fptr_src_image);
		if( feof(fptr_src_image) ) 
			break;
		fputc(ch, fptr_stego_image);
	}
	if( ftell(fptr_src_image) == 54 ) 
		return e_success;
	else
		return e_failure;		
}

/* Encode Magic String 
 * Input: MAGIC_STRING - "#*" and structure pointer encInfo
 * Output: 16B encoded from src_image and copied to dest_image
 * Description: 16B (54-69) is copied after encoding with "#*"
 * Return: e_failure or e_success 
*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{	
	if( encode_data_to_image( MAGIC_STRING, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )
		return e_success;
	else
		return e_failure;
}

/* Encode function - real encoding function
 * Input: MAGIC_STRING - "#*", strlen(MAGIC_STRING), file pointer to src_image and dest_image
 * Output: encoded data to image
 * Description: read 8 bytes into buffer and pass it to  ->  encode_byte_to_lsb(char data, char *image_buffer);
 * Return: e_failure or e_success 
*/
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	int i, flag = 0;
	char buffer[8];
	for( i = 0; i < size; i++)
	{
		fread( buffer, sizeof(buffer), 1, fptr_src_image );				//read 8bytes at a time into buffer from src_image
		if( encode_byte_to_lsb( data[i], buffer) == e_success )			//encode the LSB 
			flag = 1;
		fwrite( buffer, sizeof(buffer), 1, fptr_stego_image );			//write 8 bytes to dest_image
	}	
	if( flag == 1 ) 
		return e_success;
	else 
		return e_failure;
}


/* Encode a byte into LSB of image data array
 * Input: MAGIC_STRING - "#*" and image_buffer
 * Output: encode byte to lsb
 * Description: encode by storing bits from data to lsb of bytes from  image
 * Return: e_failure or e_success 
*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	int i;
	char bit;
	for( i = 0; i < 8; i++ )
	{	
		bit = !!( (0x80 >> i) & data);							//fetch each bit from data
		image_buffer[i] = ( image_buffer[i] & ~(0x01)) | bit;	//replace the LSB of image bytes
	}	
	return e_success;
}

/* Encoding size of extn and file 
 * Input: size of extn/ file, file pointer to src_image & dest_image
 * Output: get 4*8=32 B from src_image and pass it to - encode_size_to_lsb()
 * Description: fetch 32B and pass it encoding and store modified data in dest_image
 * Return: e_failure or e_success 
*/
Status encode_size( int size, FILE *fptr_src_image, FILE *fptr_stego_image )
{
	char image_buffer[32];
	int flag = 0;
	fread( image_buffer, sizeof(image_buffer), 1, fptr_src_image );							//read 32B at a time
	if( encode_size_to_lsb( size, image_buffer) == e_success ) 								//encode size of file extn
	{	fwrite(  image_buffer, sizeof(image_buffer), 1, fptr_stego_image  );				//write to dest_image
		flag = 1;
	}
	else
		flag = 0;		
	if( flag == 1 )
		return e_success;
	else
		return e_failure;
}

/* Encode extension size to lsb 
 * Input: size of extn/ file, image_buffer 32B
 * Description: encode bits from MSB of size to lsb of all 32 bytes
 * Return: e_failure or e_success 
*/
Status encode_size_to_lsb( int size, char *buffer )
{
	int i;
	char bit;
	for( i = 0; i < 32; i++ )
	{	
		bit = !!( (0x80000000 >> i) & size);					//get the bits from 'size'
		buffer[i] = ( buffer[i] & ~(0x01)) | bit;				//replace the LSB of image bytes
	}	
	return e_success;
}

/* Encode secret file extenstion
 * Input: size of extn/ file, image_buffer 32B
 * Description: encode bits from MSB of size to lsb of all 32 bytes
 * Return: e_failure or e_success 
*/
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{	
	if( encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image ) == e_success)
		return e_success;
	else 
		return e_failure;	
}

/* Encode secret file size
 * Input: size of file, structure pointer
 * Description: encode bits from MSB of size to lsb of all 32 bytes
 * Return: e_failure or e_success 
*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{	
	if( encode_size( file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image ) == e_success )
		return e_success;
	else
		return e_failure;	
}

/* Encode secret file data
 * Input: structure pointer
 * Description: encode secret file data
 * Return: e_failure or e_success 
*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{	
	char ch;
	int flag = 0;
	rewind(encInfo->fptr_secret);
	while(1)
	{
		ch = fgetc(encInfo->fptr_secret);						//get byte by byte from secret_file till EOF 
		if( feof(encInfo->fptr_secret) )
			break;												//encode it into the dest_image
		if( encode_data_to_image( &ch, sizeof(ch), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )
			flag = 1;
		else
			flag = 0;		
	}
	if( flag == 1 )
		return e_success;
	else
		return e_failure;	
}


/* Copy remaining image bytes from src to stego image after encoding 
 * Input: structure pointer to src_image and dest_image
 * Description: copy remaing bytes of src_image w/o modification
 * Return: e_failure or e_success 
*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)				//copy all remaing bytes till EOF of src_image
{
	char ch;
	while(1)
	{	ch = fgetc(fptr_src);
		if( feof(fptr_src) ) 
			break;
		fputc(ch, fptr_dest);
	}
	return e_success;
}


