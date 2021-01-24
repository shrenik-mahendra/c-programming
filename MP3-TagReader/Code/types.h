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

#ifndef TYPES_H
#define TYPES_H

/* user defined macros */

#define MP3 ".mp3"			//extension of MP3 file -> .mp3
#define ID3 "ID3"			//MP3 HEADER ID -> "ID3"
#define HEAD_ID 3			//HEADER ID size -> 3B
#define MOD 10				//no of modifiers
#define MOD_LEN 2			//length of modifier
#define VER 2				//VERSION size -> 2B
#define FLAG 1				//FLAG size -> 1B
#define SIZE 4				//SIZE size -> 4B
#define FRAME_LEN 4			//FRAME HEADER size -> 4B
#define FRAME_FLAG 2		//FRAME FLAG size -> 2B
#define NUL 1				//NULL byte of FRAME CONTENT -> 1B
#define TAG 72					//Total tags to present in ID3v2.3
#define JPEG "image/jpeg"		//type of image -> .jpeg image
#define PNG "image/png"			//type of image -> .png image
#define JPG_IMG "sample.jpg"	//default name of extracted JPEG image 
#define PNG_IMG "sample.png"	//default name of extracted PNG image 

/* user defined data types */

typedef unsigned int uns;

/* Status enum is used as return type in functions to indicate SUCCESS/ FAILURE */
typedef enum		
{
    t_pass,			//t_pass -> success
    t_fail			//t_fail -> failure	
} Status;

/* OperationType enum is used as return type in functions to indicate TYPE of OPERATION */
typedef enum		
{
    t_view,			//t_view -> view MP3 tags operation
    t_edit,			//t_edit -> edit MP3 tags operation
    t_remove,		//t_remove -> remove all MP3 tags operation
    t_help,			//t_help -> to display USAGE and Modifier list
    t_invalid		//t_invalid -> invalid modifier operation
} OperationType;

#endif


