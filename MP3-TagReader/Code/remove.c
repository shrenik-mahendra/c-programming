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
#include "remove.h"

/*	Check no of files passed: REMOVE operation
*	Input: argument counter
*	Description: check the no of arguments == 3
*	Return: t_pass/ t_fail
*/
Status check_remove_file(int argc)
{
	if( argc != 3)
		return t_fail;
	else
		return t_pass;	
}

/*	Validate MP3 file extension
*	Input: argument pointer, structure pointer to RemoveTag
*	Description: check extension of file passed is ".mp3"
*	Return: t_pass/ t_fail
*/
Status validate_remove_file(char **argv, RemoveTag *tagRemove)
{
	char *str = argv[2] + strlen(argv[2]) - strlen(MP3);			//str -> points to extension .mp3
	if( strcmp(str, MP3) == 0 )
	{	
		tagRemove->fname = (char *) malloc(strlen(argv[2]) + 1);		//store MP3 file name
		strcpy(tagRemove->fname, argv[2]);
		return t_pass;
	}
	else
		return t_fail;
}

/*	Remove TAGS function
*	Input: structure pointer to RemoveTag
*	Description: open file -> check header -> check version 
*	Output: get the version of MP3 file
*	Return: t_pass/ t_fail
*/
Status do_tag_remove(RemoveTag *tagRemove)
{
	if( open_remove_file(tagRemove) == t_pass )
	{
		rewind(tagRemove->fptr_remove);
		if( check_mp3_header(tagRemove->fptr_remove, tagRemove->fptr_dummy) == t_pass )
		{
			printf("MP3 TAG Reader & Editor\n");
			printf("-----------------------\n");
			if( check_mp3_version(tagRemove->fptr_remove, tagRemove->fptr_dummy) == t_pass )
			{
				if( remove_tags(tagRemove->fptr_remove, tagRemove->fptr_dummy, tagRemove) == t_pass )
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
		printf("ERROR: ./mp3_tag : Error Opening '%s'\n",tagRemove->fname);
		return t_fail;
	}
}

/*	Open MP3 file
*	Input: structure pointer to RemoveTag
*	Description: open file using file pointer & check size of file
*	Output: get size of MP3 file
*	Return: t_pass/ t_fail
*/
Status open_remove_file(RemoveTag *tagRemove)
{
	tagRemove->fptr_remove  = fopen(tagRemove->fname, "r");								//open file in READ mode
	if( tagRemove->fptr_remove == NULL )									
		return t_fail;
	else
	{	
		fseek(tagRemove->fptr_remove, 0, SEEK_END);										//move file pointer to END: get size
		if( ( tagRemove->file_size = ftell(tagRemove->fptr_remove) ) == 0 )				//store size of mp3 file
		{
			printf("ERROR: ./mp3_tag : '%s' is empty\n", tagRemove->fname);
			return t_fail;
		}
		else
		{	
			tagRemove->fptr_dummy = fopen("dummy.mp3", "w");
			return t_pass;
		}
	}
}

/*	Check HEADER ID
*	Input: file pointer to MP3 file, file pointer to dummy.mp3
*	Description: check HEADER ID == "ID3" -> 3 Bytes
*	Return: t_pass/ t_fail
*/
Status check_mp3_header(FILE *fptr_remove, FILE *fptr_dummy)
{
	char head[HEAD_ID];												//head -> buffer to store HEADER ID
	fread(head, sizeof(head), 1, fptr_remove);
	if( strncmp(head, ID3, strlen(ID3)) == 0 )
	{
		fwrite(head, sizeof(head), 1, fptr_dummy);
		return t_pass;
	}
	else
		return t_fail;
}

/*	Check ID3 version of both MP3 files
*	Input: file pointer to MP3 file, file pointer to dummy.mp3
*	Description: check version == 03 xx  --> v2.3.x
*	Output: get version of ID3
*	Return: t_pass/ t_fail
*/
Status check_mp3_version(FILE *fptr_remove, FILE *fptr_dummy)
{
	char version[VER],size_buff[SIZE+FLAG];												//version -> buffer to store version bytes
	fread(version, sizeof(version), 1, fptr_remove);
	if( version[0] == 0x03)
	{
		printf("%-15s: %d.%d.%d\n","Version ID", 2, 3, version[1]);
		fwrite(version, sizeof(version), 1, fptr_dummy);
		fread(size_buff, sizeof(size_buff), 1, fptr_remove);
		fwrite(size_buff, sizeof(size_buff), 1, fptr_dummy);
		return t_pass;
	}
	else
		return t_fail;
}

/*	Main function to Remove tags
*	Input: file pointer to original MP3 file, file pointer to dummy.mp3, structure pointer to RemoveTag
*	Description: Delete all the TAGS till "APIC" from original MP3 file
*	Return: t_pass/ t_fail
*/
Status remove_tags(FILE *fptr_remove, FILE *fptr_dummy, RemoveTag *tagRemove)
{
	int i, size;
	char frame_head[FRAME_LEN], size_buff[SIZE];
	for( i = 0; i < TAG; i++ )	
	{
		fread(frame_head, sizeof(frame_head), 1, fptr_remove);					
		if( strncmp(frame_head, "APIC", FRAME_LEN) == 0)						//if FRAME_HEADER == APIC							
		{	
			fseek(fptr_remove, -FRAME_LEN, SEEK_CUR);
			copy_remaining_bytes(fptr_remove, fptr_dummy, tagRemove);			//copy remaining bytes function call
			break;	
		}
		else
		{
			fread(size_buff, sizeof(size_buff), 1, fptr_remove);		
			size = get_size_in_int(size_buff);									//get size of FRAME_CONTENT
			fseek(fptr_remove, FRAME_FLAG+size, SEEK_CUR);						//skip FRAME_FLAG + FRAME_CONTENT bytes
		}														
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
uns get_size_in_int(char *size_buff)
{
	uns size = 0, i;
	for( i = 0; i< SIZE; i++ )											
		size = size | ( (unsigned char)size_buff[i] << (24 - (i*8)) );		
	return size;
}

/*	Convert size of FRAME to Int
*	Input: size array of 4B
*	Description: Convert size from char array of 4 bytes to int val
*	Return: size of frame as int val
*/
Status copy_remaining_bytes(FILE *fptr_remove, FILE *fptr_dummy, RemoveTag *tagRemove)
{
	char ch;
	while(1)
	{
		ch = fgetc(fptr_remove);						//copy data till EOF of original MP3 to dummy.mp3
		if( feof(fptr_remove) )
			break;
		fputc(ch, fptr_dummy);
	}
	fclose(fptr_remove);
	fclose(fptr_dummy);
	fptr_remove  = fopen(tagRemove->fname, "w");		//reopen original MP3 in write mode
	fptr_dummy = fopen("dummy.mp3", "r");				//reopen dummy.mp3 in read mode
	while(1)
	{
		ch = fgetc(fptr_dummy);
		if( feof(fptr_dummy) )							//copy all bytes from dummy.mp3 to original to MP3
			break;
		fputc(ch, fptr_remove);
	}
	return t_pass;
}


