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
#include "types.h"
#include "view.h"
#include "edit.h"
#include "remove.h"

void print_modifiers_list(void);

int main(int argc, char *argv[])	//argc -> argument counter; argv -> argument pointer
{
	int state, type;
	ViewTag tagView;				//tagView -> structure variable to ViewTag
	EditTag tagEdit;				//tagEdit -> structure variable to EditTag
	RemoveTag tagRemove;			//tagRemove -> structure variable to RemoveTag
	if( argc == 1 )					//check if no arguments passed
	{
		printf("Error: ./mp3_tag : Invalid Arguments Passed\n");
		printf("Usage: ./mp3_tag -h : For Help\n");
		return 0;
	}
	else
	{
		type = check_operation_type(argv);						//check operation type passed
		if( type == t_view )									//opration type -> view MP3 tags
		{
			state = check_view_file(argc);						//check no of arguments passed
			if( state == t_pass )
			{
				state = validate_view_file(argv, &tagView);		//validate .mp3 extension & store file name
				if( state == t_pass )
				{
					state = do_tag_view(&tagView);				//function call to view tags
					if( state == t_pass )
					{
						printf("\nExtracting MP3 TAGS Completed\n");
						fclose(tagView.fptr_view);
						fclose(tagView.fptr_image);
						return 0;
					}
					else
					{	
						printf("ERROR: ./mp3_tag : Extracting MP3 TAGS of '%s' failure\n",tagView.fname);
						return 1;
					}
				}
				else
				{
					printf("ERROR: ./mp3_tag : Invalid File Passed\n");
					printf("To View TAGS: USAGE: ./mp3_tag -v <sample.mp3 file>\n\n");
					return 1;
				}
			}
			else
			{
				printf("ERROR: ./mp3_tag : Invalid no. of Files Passed\n");
				printf("To View TAGS: USAGE: ./mp3_tag -v <sample.mp3 file>\n\n");
				return 1;
			}
		}
		else if( type == t_edit )											//operation type -> edit MP3 tags
		{
			state = check_edit_file(argc);									//check no of arguments passed
			if( state == t_pass )
			{
				state = validate_edit_file(argv, &tagEdit);					//validate .mp3 extension & store file name
				if( state == t_pass )
				{
					state = check_modifier(argv, &tagEdit);					//check and validate the modifier passed
					if( state == t_pass )
					{
						if( do_tag_edit(&tagEdit) == t_pass)				//function call to edit the tag based on modifier passed
						{	
							printf("\nEditing '%s' TAG Completed\n", tagEdit.frame);
							fclose(tagEdit.fptr_edit);
							return 0;
						}
						else
						{
							printf("\nERROR: ./mp3_tag : Editing '%s' TAG of '%s' failure\n",tagEdit.frame, tagEdit.fname);
							return 1;	
						}
					}
					else
					{
						printf("ERROR: ./mp3_tag : Invalid Modifier Passed\n\n");
						print_modifiers_list();
						return 1;
					}
				}
				else
				{
					printf("ERROR: ./mp3_tag : Invalid File Passed\n");
					printf("To Edit TAGS: USAGE: ./mp3_tag -e <sample.mp3 file> <modifier> <TAG>\n\n");
					return 1;
				}
			}
			else
			{
				printf("ERROR: ./mp3_tag : Invalid no. of Files Passed\n");
				printf("To Edit TAGS: USAGE: ./mp3_tag -e <sample.mp3 file> <modifier> <TAG>\n\n");
				return 1;
			}
		}
		else if( type == t_remove )											//operation type -> remove tags
		{
			state = check_remove_file(argc);								//check no of arguments passed
			if( state == t_pass )
			{
				state = validate_remove_file(argv, &tagRemove);				//validate .mp3 extension & store file name
				if( state == t_pass )
				{
					state = do_tag_remove(&tagRemove);						//function call to remove all MP3 tags
					if( state == t_pass )
					{
						printf("\nRemoving MP3 TAGS of '%s' completed\n", tagRemove.fname);
						fclose(tagRemove.fptr_remove);
						fclose(tagRemove.fptr_remove);
						return 0;
					}
					else
					{
						printf("\nERROR: Removing MP3 TAGS of '%s' failure\n", tagRemove.fname);
						
					}
				}
				else
				{
					printf("ERROR: ./mp3_tag : Invalid File Passed\n");
					printf("To Remove TAGS: USAGE: ./mp3_tag -r <sample.mp3 file>\n\n");
					return 1;	
				}				 
			}
			else
			{
				printf("ERROR: ./mp3_tag : Invalid no. of Files Passed\n");
				printf("To Remove TAGS: USAGE: ./mp3_tag -r <sample.mp3 file>\n\n");
				return 1;
			}
		}
		else if( type == t_help )										//operation type -> help menu
		{
			if( argc > 2 )												//check if no of arguments passed == 2
			{
				printf("Error: ./mp3_tag : Invalid Arguments Passed\n");
				printf("Usage: ./mp3_tag -h : For Help\n");
				return 1;
			}
			else
			{
				printf("MP3 TAG Reader & Editor : Help Menu\n\n");
				printf("To View TAGS: USAGE: ./mp3_tag -v <sample.mp3 file>\n\n");
				printf("To Edit TAGS: USAGE: ./mp3_tag -e <sample.mp3 file> <modifier> <TAG>\n\n");
				printf("To Remove TAGS: USAGE: ./mp3_tag -r <sample.mp3 file>\n\n");
				print_modifiers_list();									// function call to print modifiers list
				return 0;
			}
		}	
		else
		{
			printf("Error: ./mp3_tag : Invalid Operation Type\n");
			printf("Usage: ./mp3_tag -h : For Help\n");
			return 1;
		}
	}
	return 1;	
}

/*	Print modifiers list for EDIT option 
*	Input: void
*	Description: Print list of modifiers supported during -e operation
*	Output: Modifiers List for -e opration
*	Return: void
*/
void print_modifiers_list(void)
{
	printf("List of Modifiers\n\n%8s\t%8s\n", "Modifier", "Function");
	printf("%5s%3s\t%s\n", "-a", " ", "TALB: Modifies Album TAG");
	printf("%5s%3s\t%s\n", "-c", " ", "TCOM: Modifies Composer TAG");
	printf("%5s%3s\t%s\n", "-g", " ", "TCON: Modifies Genre TAG");
	printf("%5s%3s\t%s\n", "-t", " ", "TIT2: Modifies Title TAG");
	printf("%5s%3s\t%s\n", "-l", " ", "TPE1: Modifies Lead Performer TAG");
	printf("%5s%3s\t%s\n", "-b", " ", "TPE2: Modifies Band TAG");
	printf("%5s%3s\t%s\n", "-p", " ", "TPUB: Modifies Publisher TAG");
	printf("%5s%3s\t%s\n", "-n", " ", "TRCK: Modifies Track Number TAG");
	printf("%5s%3s\t%s\n", "-y", " ", "TYER: Modifies Year TAG");
	printf("%5s%3s\t%s\n", "-m", " ", "COMM: Modifies Comment TAG");
}

