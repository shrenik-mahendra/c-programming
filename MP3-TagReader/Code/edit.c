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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "edit.h"

/* 2D array of all the modifiers for -e option */
char modifier[][3] = {"-a", "-c", "-g", "-t", "-l", "-b", "-p", "-n", "-y", "-m"};

/* 2D array of all the FRAME HEADERS to print content */
char frame_edit[][5] = {"TALB", "TCOM", "TCON", "TIT2", "TPE1", "TPE2", "TPUB", "TRCK", "TYER", "COMM"};

/*	Check no of files passed: Edit operation
*	Input: argument counter
*	Description: check the no of arguments == 5
*	Return: t_pass/ t_fail
*/
Status check_edit_file(int argc)
{
	if( argc != 5 )
		return t_fail;
	else
		return t_pass;
}

/*	Validate MP3 file extension
*	Input: argument pointer, structure pointer to EditTag
*	Description: check extension of file passed is ".mp3" & store file name in structure
*	Return: t_pass/ t_fail
*/
Status validate_edit_file(char **argv, EditTag *tagEdit)
{
	char *str = argv[2] + strlen(argv[2]) - strlen(MP3);		//str -> points to extension .mp3
	if( strcmp(str, MP3) == 0 )
	{
		tagEdit->fname = (char *) malloc(strlen(argv[2]) + 1);		//store MP3 file name
		strcpy(tagEdit->fname, argv[2]);
		return t_pass;
	}
	else
		return t_fail;
}

/*	Check and validate TAG modifier
*	Input: argument pointer, structure pointer to EditTag
*	Description: check the <modifier> passed belongs to array in 'modifier[][]'
*	Return: t_pass/ t_fail
*/
Status check_modifier(char **argv, EditTag *tagEdit)
{
	char *str = argv[3];
	int i, flag = 0;								//flag = 1: FRAME for the modifier passed is present in the MP3 file
	for( i = 0; i < MOD; i++)						//run loop for no of modifier times
	{
		if( strcmp(str, modifier[i]) == 0 )						//if modifier is valid
		{
			strcpy(tagEdit->modifier, str);									//store the modifier in the structure
			strcpy(tagEdit->frame, frame_edit[i]);							//store the respective frame in the structure
			tagEdit->new_tag = (char *) malloc( strlen(argv[4]) + 1 );		//store the NEW frame content
			strcpy(tagEdit->new_tag, argv[4]);
			tagEdit->new_tag_size = strlen(tagEdit->new_tag) + 1;			//store the size of NEW frame content
			flag = 1;
			break;
		}
	}
	if( flag == 1 )
		return t_pass;
	else									//flag = 0: FRAME for the modifier passed is not present in the MP3 file
		return t_fail;
}

/*	Edit TAGS function
*	Input: structure pointer to EditTag
*	Description: open file -> check header -> check version
*	Output: get the version of MP3 file
*	Return: t_pass/ t_fail
*/
Status do_tag_edit(EditTag *tagEdit)
{
	if( open_edit_file(tagEdit) == t_pass )									//open file
	{
		rewind(tagEdit->fptr_edit);											//rewind the file_pointer to START
		if( check_header_id(tagEdit->fptr_edit) == t_pass )					//check HEADER_ID
		{
			if( check_version_id(tagEdit->fptr_edit) == t_pass )			//check VERSION
			{
				printf("MP3 TAG Reader & Editor\n");
				printf("-----------------------\n");
				if( edit_tag(tagEdit->fptr_edit, tagEdit) == t_pass )		//function call to edit_tag()
					return t_pass;
				else
					return t_fail;
			}
			else
			{
				printf("ERROR: Version not Matched: Pass ID3v2.3 <.mp3 file>\n");
				return t_fail;
			}
		}
		else
		{
			printf("ERROR: Header ID not Matched: Pass <.mp3 file>\n");
			return t_fail;
		}
	}
	else
	{
		printf("ERROR: ./mp3_tag : Error Opening '%s'\n",tagEdit->fname);
		return t_fail;
	}
}

/*	Open MP3 file
*	Input: structure pointer to EditTag
*	Description: open file using file pointer & check size of file
*	Output: get size of MP3 file
*	Return: t_pass/ t_fail
*/
Status open_edit_file(EditTag *tagEdit)
{
	tagEdit->fptr_edit  = fopen(tagEdit->fname, "r");						//open file in READ mode
	if( tagEdit->fptr_edit == NULL )
		return t_fail;
	else
	{
		fseek(tagEdit->fptr_edit, 0, SEEK_END);								//move file pointer to END: get size
		if( ( tagEdit->file_size = ftell(tagEdit->fptr_edit) ) == 0 )		//store size of mp3 file
		{
			printf("ERROR: ./mp3_tag : '%s' is empty\n", tagEdit->fname);
			return t_fail;
		}
		else
			return t_pass;
	}
}

/*	Check HEADER ID
*	Input: file pointer to MP3 file
*	Description: check HEADER ID == "ID3" -> 3 Bytes
*	Return: t_pass/ t_fail
*/
Status check_header_id(FILE *fptr)
{
	char head[HEAD_ID];									//head -> buffer to store HEADER ID
	fread(head, sizeof(head), 1, fptr);
	if( strncmp(head, ID3, strlen(ID3)) == 0 )
		return t_pass;
	else
		return t_fail;
}

/*	Check ID3 version
*	Input: file pointer to MP3 file
*	Description: check version == 03 xx  --> v2.3.x
*	Output: get version of ID3
*	Return: t_pass/ t_fail
*/
Status check_version_id(FILE *fptr)
{
	char version[VER];												//version -> buffer to store version bytes
	fread(version, sizeof(version), 1, fptr);
	if( version[0] == 0x03)
	{
		fseek(fptr, FLAG+SIZE, SEEK_CUR);								//skip the FLAG (1B)
		return t_pass;
	}
	else
		return t_fail;
}

/*	Edit TAG main function
*	Input: file pointer to MP3 file, structure pointer to EditTag
*	Description: change the FRAME content of the respective FRAME of the modifier passed
*	Output: updated frame content for the MP3 tag modifier
*	Return: t_pass/ t_fail
*/
Status edit_tag(FILE *fptr, EditTag *tagEdit)
{
	char frame_head[FRAME_LEN], size_buff[SIZE];			//frame_head -> buffer to store FRAME Header;  size_buff -> store 4B of SIZE bytes
	int i, flag = 0;
	for( i = 0; i < TAG; i++ )								//run for the total no of TAGS preseent in MP3 file (72)
	{
		fread(frame_head, sizeof(frame_head), 1, fptr);			//read the FRAME HEADER
		tagEdit->new_tag_pos = ftell(fptr);						//store the location of file pointer
		fread(size_buff, sizeof(size_buff), 1, fptr);			//read 4B of SIZE
		tagEdit->frame_size = conv_size(size_buff);				//store the interger val of size_buff[]
		fseek(fptr, FRAME_FLAG, SEEK_CUR);						//skip the FRAME_FLAG (2B)
		if( strncmp(frame_head, "APIC", FRAME_LEN) == 0 )						//if the FRAME HEADER reached "APIC": attached image
			break;
		else if( strncmp(frame_head, tagEdit->frame, FRAME_LEN) == 0 )			//if FRAME of the modifier passed is found
		{
			if( edit_tag_content(tagEdit, tagEdit->fptr_edit, tagEdit->fptr_dummy) == t_pass )
			{														//function call to EDIT TAG
				flag = 1;
				break;
			}
		}
		else
			fseek(fptr, tagEdit->frame_size, SEEK_CUR);					//skip SIZE bytes  i.e, skip thet frame contents
	}
	if( flag == 1 )
		return t_pass;
	else
	{
		printf("ERROR: ./mp3_tag : '%s' TAG not found in '%s' --> Please try to modify other TAG\n\n", tagEdit->frame, tagEdit->fname);
		print_modifiers_list();			 //call function to print modifiers list
		return t_fail;
	}
}

/*	Convert size of FRAME to Int
*	Input: size array of 4B
*	Description: Convert size from char array of 4 bytes to int val
*	Return: size of frame as int val
*/
/* example:
size_arr[] = {0x12, 0x34, 0x56, 0x78}
LS 0x12 << 24 | size: size = 12 00 00 00
LS 0x34 << 16 | size: size = 12 34 00 00
LS 0x56 <<  8 | size: size = 12 34 56 00
LS 0x78 <<  0 | size: size = 12 34 56 78 //int val
*/
uns conv_size(char *size_buff)
{
	uns size = 0, i;
	for( i = 0; i< SIZE; i++ )
		size = size | ( (unsigned char)size_buff[i] << (24 - (i*8)) );
	return size;
}

/*	Edit tag content fot the frame
*	Input: structurepointer to EditTag, file_pointer to MP3 file, file_pointer to dummy.mp3 file
*	Description: edit the FRAME CONTENT by creating the dummy.mp3 and then copy it to original MP3 file
*	Return: t_pass/ t_fail
*/
Status edit_tag_content(EditTag *tagEdit, FILE *fptr_edit, FILE *fptr_dummy)
{
	int i;
	char ch, frame_flag[FRAME_FLAG+NUL], size_buff[SIZE];	//frame_flag -> to read the FRAME_FLAG & NULL bytes; size_buff -> to read 4B of size
	fclose(fptr_edit);										//close the original MP3 file											
	fptr_edit = fopen(tagEdit->fname, "r");					//reopen the original MP3 file in read mode
	fptr_dummy = fopen("dummy.mp3", "w");					//open dummy.mp3 in write mode
	for( i = 0; i < tagEdit->new_tag_pos; i++ )
	{
		ch = fgetc(fptr_edit);								//copy the file contents of MP3 to dummy till 'new_tag_pos'				
		fputc(ch, fptr_dummy);
	}
	conv_size_from_int(tagEdit->new_tag_size, size_buff);			//function call to convert size from int to char array of 4B
	fwrite(size_buff, sizeof(size_buff), 1, fptr_dummy);			//copy size_buff array to dummy.mp3
	fread(frame_flag, sizeof(frame_flag), 1, fptr_edit);					//read the FRAME_FLAG & NULL bytes
	fwrite(frame_flag, sizeof(frame_flag), 1, fptr_dummy);					//copy FRAME_FLAG & NULL bytes
	fwrite(tagEdit->new_tag, strlen(tagEdit->new_tag), 1, fptr_dummy);		//copy the frame content to dummy.mp3
	fseek(fptr_edit, tagEdit->frame_size+FRAME_FLAG+NUL, SEEK_CUR);			//skip the FRAME_FLAG+NUL+FRAME_CONTENT bytes of original MP3 file
	while(1)
	{
		ch = fgetc(fptr_edit);
		if( feof(fptr_edit) )					//copy the remaining bytes of original MP3 file to dummy.mp3 
			break;
		fputc(ch, fptr_dummy);
	}
	fclose(fptr_edit);									//close original MP3 file
	fclose(fptr_dummy);									//close dummy.mp3
	fptr_edit = fopen(tagEdit->fname, "w");				//reopen original MP3 file in "write" mode
	fptr_dummy = fopen("dummy.mp3", "r");				//reopen dummy.mp3 in "read" mode
	while(1)
	{
		ch = fgetc(fptr_dummy);							//copy complete dummy.mp3 file into the oroginal MP3 file
		if( feof(fptr_dummy) )
			break;
		fputc(ch, fptr_edit);
	}
	fclose(fptr_edit);									//close original MP3 file
	fclose(fptr_dummy);									//close dummy.mp3
	fptr_edit = fopen(tagEdit->fname, "r");						//reopen original MP3 file in "read" mode
	fseek(fptr_edit, strlen(ID3)+VER+FLAG+SIZE, SEEK_SET);		//move the file_pointer to the 1st FRAME_HEADER
	print_updated_tags(fptr_edit, tagEdit);							//function call to print the updated tags
	return t_pass;
}

/*	Convert size of FRAME to 4B of char array
*	Input: integer value of size
*	Description: Convert size from int to 4B of char array to match BEM format -> MSB to LSB
*	Return: t_pass/ t_fail
*/
/* example:
size = 0x12345678 : in memory : LSB to MSB -> 78 56 34 12  : ptr = &size
ptr+3 -> ptr points to 78: size_buff[3] = 78
ptr+2 -> ptr points to 56: size_buff[2] = 56
ptr+1 -> ptr points to 34: size_buff[1] = 34
ptr+0 -> ptr points to 12: size_buff[0] = 12
*/
Status conv_size_from_int(uns size, char *size_buff)
{
	void *ptr = &size;
	for( int i = 0; i < SIZE; i++ )
		size_buff[SIZE-i-1] = *(unsigned char *)(ptr+i);
	return t_pass;
}

/*	Function to print Updated TAGS
*	Input: file pointer to MP3 file, structure pointer
*	Description: read FRAME_HEADER & SIZE bytes: If, FRAME_HEADER does not below to list
				 '  char frame_view[][] ' then skip the the FRAME_CONTENT bytes.
*	Return: t_pass/ t_fail
*/
Status print_updated_tags(FILE *fptr, EditTag *tagEdit)
{
	int i, size;														//size -> to store int val of size
	char frame_head[FRAME_LEN], size_buff[SIZE];						//frame_head -> buffer to store Frame_header (4B)
	for( i = 0; i < TAG; i++ )											//size_buff -> buffer to store SIZE bytes (4B)
	{
		fread(frame_head, sizeof(frame_head), 1, fptr);					//read FRAME_HEADER
		if( strncmp(frame_head, "APIC", FRAME_LEN) == 0)				// APIC -> attached picture: break
			break;														// break after extracting APIC info: as it is last tag
		fread(size_buff, sizeof(size_buff), 1, fptr);					//read SIZE
		size = conv_size(size_buff);									// function call to conv_size()
		fseek(fptr, FRAME_FLAG+NUL, SEEK_CUR);									//skip FRAME_FLAG & NULL of FRAME CONTENT
		if( print_tag(frame_head, fptr, size-NUL) == t_fail )			//function call to print_tag()
			fseek(fptr, size-NUL, SEEK_CUR);									//skip FRAME_CONTENT bytes
	}
	return t_pass;
}

/*	Print FRAME CONTENTs
*	Input: Frame_Header, file pointer, SIZE-NUL
*	Description: check for the FRAME_HEADER in
				 '  char frame_edit[][] ' and print if present
*	Output: TAG/ FRAME HEADER content
*	Return: t_pass/ t_fail
*/
Status print_tag( char *frame_head, FILE *fptr, int size )
{
	char buffer[size];												//buffer -> to store the FRAME CONTENT
	int i, flag = 0, tag;											// flag = 1: FRAME found; tag -> row index of FRAME in frame_view[][]
	for( i = 0; i < MOD; i++ )
	{
		if( strncmp(frame_head, frame_edit[i], FRAME_LEN) == 0 )	//check for the FRAME_HEADER in frame_view[][]
		{
			flag = 1;
			tag = i;
			break;
		}
	}
	if( flag == 1 )
	{	fread(buffer, size, 1, fptr);
		switch(tag)
		{
			case 0: printf("%-15s: ","Album");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 1: printf("%-15s: ","Composer");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 2: printf("%-15s: ","Genre");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 3: printf("%-15s: ","Title");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 4: printf("%-15s: ","Lead Performer");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 5: printf("%-15s: ","Band");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 6: printf("%-15s: ","Publisher");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 7: printf("%-15s: ","Track Number");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 8: printf("%-15s: ","Year");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
			case 9: printf("%-15s: ","Comment");
					fwrite(buffer, size, 1, stdout);
					printf("\n");
					break;
		}
		return t_pass;
	}
	else
		return t_fail;
}

