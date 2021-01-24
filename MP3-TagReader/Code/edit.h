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


#ifndef EDIT_H
#define EDIT_H

#include "types.h"

/* Structure to store FILE info */
typedef struct Edit_Tag
{
	/* members containing the details of old FRAME/ TAG/ FRAME CONTENT */
	char *fname;					//name of MP3 file
	char modifier[MOD_LEN+1];		//store the modifier passed
	char frame[FRAME_LEN+1];		//store the respective FRAME
	FILE *fptr_edit;				//file pointer to 'fname'
	uns file_size;					//store size of file
	uns frame_size;					//size of frame_content of modifier passed
	
	
	/* members containing the details of new FRAME/ TAG/ FRAME CONTENT */
	char *new_tag;					//store the NEW frame content
	int new_tag_size;				//store the size of NEW frame content
	int new_tag_pos;				//store the position of the frame[] in the file
	FILE *fptr_dummy;				//file pointer to DUMMY file to copy file content
			
}EditTag;

/*	Check no of files passed: Edit operation */
Status check_edit_file(int argc);

/*	Validate MP3 file extension */
Status validate_edit_file(char **argv, EditTag *tagEdit);

/*	Check and validate TAG modifier */
Status check_modifier(char **argv, EditTag *tagEdit);

/*	Edit TAGS function */
Status do_tag_edit(EditTag *tagEdit);

/*	Open MP3 file */
Status open_edit_file(EditTag *tagEdit);

/*	Check HEADER ID */
Status check_header_id(FILE *fptr);

/*	Check ID3 version */
Status check_version_id(FILE *fptr);

/*	Edit TAG main function */
Status edit_tag(FILE *fptr, EditTag *tagEdit);

/*	Convert size of FRAME to Int */
uns conv_size(char *size_buff);

/* Print list of modifiers */
void print_modifiers_list(void);

/* Edit Frame content of the TAG modifier */
Status edit_tag_content(EditTag *tagEdit, FILE *fptr_edit, FILE *fptr_dummy);

/* Convert size from INT to CHAR array of 4B */
Status conv_size_from_int(uns size, char *size_buff);

/* Print the updated TAGS */
Status print_updated_tags(FILE *fptr, EditTag *tagEdit);

/*	Print FRAME CONTENTs */
Status print_tag( char *frame_head, FILE *fptr, int size );

#endif


