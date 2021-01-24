/* 
Author: Shrenik Mahendra
Batch: 20001 - ECEP Intermediate - JAN 2020
Description: MP3 TAG READER & EDITOR 
Date: 19 - JUL - 2020 (last updated)
Contact: 
			Ph - 9845617297
			Email - shrenik2912@gmail.com
*/

/* gcc main.c view.c edit.c remove.c -o mp3_tag    ----> to get the output file */
/* mp3_tag -h    -----> help menu */

#ifndef VIEW_H
#define VIEW_H

#include "types.h"

/* Structure to store FILE info */
typedef struct View_Tag
{
	/* members containing details of MP3 file */
	char *fname;				//name of MP3 file
	FILE *fptr_view;			//file pointer to 'fname'
	uns file_size;				//size of MP3 file
	
	/* members containing details of IMAGE file */
	char *image_name;			//name of image file
	char *image_type;			//store image type
	FILE *fptr_image;			//file pointer to 'image_name'
	uns  image_size;			//size of image
}ViewTag;

/*	Check operation type */
OperationType check_operation_type(char **argv);

/*	Check no of files passed: VIEW operation */
Status check_view_file(int argc);

/*	Validate MP3 file extension */
Status validate_view_file(char **argv, ViewTag * tagView);

/*	View TAGS function */
Status do_tag_view(ViewTag *tagView);

/*	Open MP3 file */
Status open_view_file(ViewTag * tagView);

/*	Check HEADER ID */
Status check_header(FILE *fptr);

/*	Check ID3 version */
Status check_version(FILE *fptr);

/*	Main Function to print TAGS */
Status print_tags(FILE *fptr, ViewTag *tagView);

/*	Convert size of FRAME to Int */
uns conv_size_to_int(char *size_buff);

/*	Print FRAME CONTENTs */
Status print_tag_content( char *frame_head, FILE *fptr, int size);

/*	Get attached image info */
Status get_image_info(FILE *fptr, ViewTag *tagView);

/* Extract Album art from image */
Status extract_image(ViewTag *tagView);

#endif

