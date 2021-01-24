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
#include "view.h"

/* 2D array of all the FRAME HEADERS to print content */
char frame_view[][5] = {"TALB", "TCOM", "TCON", "TIT2", "TPE1", "TPE2", "TPUB", "TRCK", "TYER", "COMM"};

/*	Check operation type
*	Input: argument pointer
*	Description: check for the type of operation choosen: VIEW, EDIT, HELP
*	Output: operation type passed
*	Return: t_view/ t_edit/ t_help/ t_invalid
*/
OperationType check_operation_type(char **argv)
{
	if( strcmp(argv[1],"-v") == 0 )
		return t_view;
	else if( strcmp(argv[1],"-e") == 0 )
		return t_edit;
	else if( strcmp(argv[1],"-h") == 0 )
		return t_help;
	else if( strcmp(argv[1],"-r") == 0 )
		return t_remove;
	else
		return t_invalid;
}

/*	Check no of files passed: VIEW operation
*	Input: argument counter
*	Description: check the no of arguments == 3
*	Return: t_pass/ t_fail
*/
Status check_view_file(int argc)
{
	if( argc != 3)
		return t_fail;
	else
		return t_pass;		
}

/*	Validate MP3 file extension
*	Input: argument pointer, structure pointer to ViewTag
*	Description: check extension of file passed is ".mp3"
*	Return: t_pass/ t_fail
*/
Status validate_view_file(char **argv, ViewTag * tagView)
{
	char *str = argv[2] + strlen(argv[2]) - strlen(MP3);		//str -> points to extension .mp3
	if( strcmp(str, MP3) == 0 )
	{	
		tagView->fname = (char *) malloc(strlen(argv[2]) + 1);		//store MP3 file name
		strcpy(tagView->fname, argv[2]);
		return t_pass;
	}
	else
		return t_fail;	
}

/*	View TAGS function
*	Input: structure pointer to ViewTag
*	Description: open file -> check header -> check version 
*	Output: get the version of MP3 file
*	Return: t_pass/ t_fail
*/
Status do_tag_view(ViewTag *tagView)
{
	if( open_view_file(tagView) == t_pass )								//open file
	{
		rewind(tagView->fptr_view);										//rewind fptr to '0' position
		if( check_header(tagView->fptr_view) == t_pass )				//check header ID
		{
			printf("MP3 TAG Reader & Editor\n");
			printf("-----------------------\n");
			if( check_version(tagView->fptr_view) == t_pass )			//check version
			{
				if( print_tags(tagView->fptr_view, tagView) == t_pass )			//function call for print_tags()
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
		printf("ERROR: ./mp3_tag : Error Opening '%s'\n",tagView->fname);
		return t_fail;
	}	
}

/*	Open MP3 file
*	Input: structure pointer to ViewTag
*	Description: open file using file pointer & check size of file
*	Output: get size of MP3 file
*	Return: t_pass/ t_fail
*/
Status open_view_file(ViewTag * tagView)
{
	tagView->fptr_view  = fopen(tagView->fname, "r");						//open file in READ mode
	if( tagView->fptr_view == NULL )									
		return t_fail;
	else
	{	
		fseek(tagView->fptr_view, 0, SEEK_END);								//move file pointer to END: get size
		if( ( tagView->file_size = ftell(tagView->fptr_view) ) == 0 )		//store size of mp3 file
		{
			printf("ERROR: ./mp3_tag : '%s' is empty\n", tagView->fname);
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
Status check_header(FILE *fptr)
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
Status check_version(FILE *fptr)
{
	char version[VER];												//version -> buffer to store version bytes
	fread(version, sizeof(version), 1, fptr);
	if( version[0] == 0x03)
	{
		printf("%-15s: %d.%d.%d\n","Version ID", 2, 3, version[1]);
		fseek(fptr, FLAG+SIZE, SEEK_CUR);								//skip the FLAG (1B)
		return t_pass;
	}
	else
		return t_fail;
}

/*	Main Function to print TAGS
*	Input: file pointer to MP3 file, structure pointer
*	Description: read FRAME_HEADER & SIZE bytes: If, FRAME_HEADER does not below to list
				 '  char frame_view[][] ' then skip the the FRAME_CONTENT bytes.
*	Return: t_pass/ t_fail
*/
Status print_tags(FILE *fptr, ViewTag *tagView)
{
	int i, size;														//size -> to store int val of size
	char frame_head[FRAME_LEN], size_buff[SIZE];						//frame_head -> buffer to store Frame_header (4B)
	for( i = 0; i < TAG; i++ )											//size_buff -> buffer to store SIZE bytes (4B)				
	{
		fread(frame_head, sizeof(frame_head), 1, fptr);					//read FRAME_HEADER
		if( strncmp(frame_head, "APIC", FRAME_LEN) == 0)				// APIC -> attached picture: if TRUE -> extract image from file
		{
			printf("Size of '%s' = %d B\n", tagView->fname, tagView->file_size);
			get_image_info(fptr, tagView);								// function call to get attached image info
			break;														// break after extracting APIC info: as it is last tag
		}
		fread(size_buff, sizeof(size_buff), 1, fptr);					//read SIZE
		size = conv_size_to_int(size_buff);								// function call to conv_size_to_int()
		fseek(fptr, FRAME_FLAG+NUL, SEEK_CUR);									//skip FRAME_FLAG & NULL of FRAME CONTENT
		if( print_tag_content(frame_head, fptr, size-NUL) == t_fail )	//function call to print_tag_content()
			fseek(fptr, size-NUL, SEEK_CUR);									//skip FRAME_CONTENT bytes
	}
	return t_pass;
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
uns conv_size_to_int(char *size_buff)								
{
	uns size = 0, i;
	for( i = 0; i< SIZE; i++ )											
		size = size | ( (unsigned char)size_buff[i] << (24 - (i*8)) );		
	return size;
}

/*	Print FRAME CONTENTs
*	Input: Frame_Header, file pointer, SIZE-NUL
*	Description: check for the FRAME_HEADER in 
				 '  char frame_view[][] ' and print if present
*	Output: TAG/ FRAME HEADER content
*	Return: t_pass/ t_fail
*/
Status print_tag_content( char *frame_head, FILE *fptr, int size)
{
	char buffer[size];												//buffer -> to store the FRAME CONTENT
	int i, flag = 0, tag;											// flag = 1: FRAME found; tag -> row index of FRAME in frame_view[][]
	for( i = 0; i < MOD; i++ )
	{
		if( strncmp(frame_head, frame_view[i], FRAME_LEN) == 0 )	//check for the FRAME_HEADER in frame_view[][]
		{	
			flag = 1;
			tag = i;
			break;
		}
	}
	if( flag == 1 )
	{	fread(buffer, size, 1, fptr);								//read the frame content
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

/*	Get attached image info
*	Input: file pointer, structure pointer to ViewTag
*	Description: extract image type, image size, image name from the MP3 file
*	Output: image type, name and size
*	Return: t_pass/ t_fail
*/	
Status get_image_info(FILE *fptr, ViewTag *tagView)
{
	char size_buff[SIZE], image_type[strlen(JPEG)], image_name[100];		//size_buff -> to read 4B of size; image_type -> to store image format
	uns size, cnt = 0, i = 0, flag = 0;										//image_name -> to read name of image if present		
	fread(size_buff, sizeof(size_buff), 1, fptr);					//read 4B of size 
	size = conv_size_to_int(size_buff);								//get size as an interger value
	fseek(fptr, FRAME_FLAG+NUL, SEEK_CUR);							//skip the FRAME FLAG & NUL bytes
	cnt += FRAME_FLAG+NUL;											// cnt -> counter : to count the no of bytes to rewind fptr
	fread(image_type, sizeof(image_type), 1, fptr);					//read the image type: image/jpeg or image/png
	cnt += sizeof(image_type);
	tagView->image_type = (char *) malloc(strlen(JPEG)+1);			//DMA -> to store image type
	if( strncmp(image_type, JPEG, 10) == 0 )						//if image is of .jpeg type
	{
		strcpy(tagView->image_type, JPEG);							//store the image type into the structure
		fseek(fptr, NUL+NUL, SEEK_CUR);								//skip the terminating NULL of "image/jpeg" & starting NULL of IMAGE NAME
		cnt += NUL+NUL;	
		while(1)													//read the image name
		{
			image_name[i] = fgetc(fptr);
			if( image_name[i] == '\0' )
				break;
			i += 1;
		}
		if( strlen(image_name) == 0 )										//if the image name is not present
		{
			tagView->image_name = (char *) malloc(strlen(JPG_IMG)+1);
			strcpy(tagView->image_name, JPG_IMG);							//assign default name for image -> "sample.jpg"
			cnt = cnt + strlen(JPG_IMG) + 1;
			flag = 1;														//flag = 1 -> image name is not present in the MP3 file
		}	
	}
	else if( strcmp(image_type, PNG) == 0)							//if image is of .png type
	{
		strcpy(tagView->image_type, PNG);							//store the image type into the structure
		fseek(fptr, NUL, SEEK_CUR);									//skip the starting NULL of IMAGE NAME
		cnt += NUL;
		while(1)													//get the image name
		{
			image_name[i] = fgetc(fptr);
			if( image_name[i] == '\0' )
				break;
			i += 1;
		}
		if( strlen(image_name) == 0 )										//if the image name is not present 
		{
			tagView->image_name = (char *) malloc(strlen(PNG_IMG)+1);
			strcpy(tagView->image_name, PNG_IMG);							//assign default name for image -> "sample.png"
			cnt = cnt + strlen(PNG_IMG) + 1;
			flag = 1;														//flag = 1 -> image name is not present in the MP3 file
		}	
	}	
	if( flag == 0 )																//flag = 0: image name present in the MP3 file
	{
		tagView->image_name = (char *) malloc(strlen(image_name)+1);			
		strcpy(tagView->image_name, image_name);								//store the image name in the structure
		cnt = cnt + strlen(image_name) + 1;
	}
	tagView->image_size = size - cnt + 1;										//size of image 
	printf("\nAttached Image Info:\n");											//print all image details
	printf("%-10s : %s\n", "Type", tagView->image_type);
	printf("%-10s : %s\n", "Name", tagView->image_name);
	printf("%-10s : %d B\n", "Size", tagView->image_size);
	printf("Open '%s' to view attached image\n", tagView->image_name);
	extract_image(tagView);														//function call to extract image art
	return t_pass;
}	

/*	Extract image from MP3 file
*	Input: structure pointer to ViewTag
*	Description: open image file & add image bytes from MP3 file
*	Output: Image embedded in MP3 file
*	Return: t_pass/ t_fail
*/
Status extract_image(ViewTag *tagView)
{
	tagView->fptr_image = fopen(tagView->image_name, "w");			//open image file in WRITE mode
	char ch;
	for( int i = 0; i <= tagView->image_size; i++ )					//run loop till size of image
	{
		ch = fgetc(tagView->fptr_view);								//copy bytes from MP3 file
		fputc(ch, tagView->fptr_image);								//write into the IMAGE file
	}
	return t_pass;
}

