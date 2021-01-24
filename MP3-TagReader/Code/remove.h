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

#ifndef REMOVE_H
#define REMOVE_H

#include "types.h"

typedef struct Remove_tag
{
	/* members containing the details of original MP3 file */
	char *fname;			//store name of MP3 file
	FILE *fptr_remove;		//file pointer to 'fname'
	uns file_size;			//size of MP3 file
	
	/* member containg details of dummy.mp3 file */
	FILE *fptr_dummy;		//file pointer to dummy.mp3
}RemoveTag;

/*	Check no of files passed */
Status check_remove_file(int argc);

/*	Validate MP3 file extension */
Status validate_remove_file(char **argv, RemoveTag *tagRemove);

/*	Remove TAGS function */
Status do_tag_remove(RemoveTag *tagRemove);

/*	Open MP3 file */
Status open_remove_file(RemoveTag *tagRemove);

/*	Check HEADER ID */
Status check_mp3_header(FILE *fptr_remove, FILE *fptr_dummy);

/*	Check ID3 version */
Status check_mp3_version(FILE *fptr_remove, FILE *fptr_dummy);

/* Main function to remove TAGS */
Status remove_tags(FILE *fptr_remove, FILE *fptr_dummy, RemoveTag *tagRemove);

/*	Convert size of FRAME to Int */
uns get_size_in_int(char *size_buff);

/* Copy bytes till end of file */
Status copy_remaining_bytes(FILE *fptr_remove, FILE *fptr_dummy, RemoveTag *tagRemove);

#endif


