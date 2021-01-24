#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main( int argc, char *argv[] )		//argc -> argumeent counter;  *argv[] -> argument variable (array of strings)
{
    EncodeInfo encInfo;					//encInfo -> structure variable of EncodeInfo
    DecodeInfo decInfo;					//decInfo -> structure variable of DecodeInfo
    int type, state;
    if( argc == 1 )
    {	printf("ERROR: Pass files from command line\n");
    	return 0;
    }
    type = check_operation_type(argv);
	if( type == e_encode)							//encoding operation
	{	
		printf("INFO: Operation Type - Encoding\n");
		state = check_encode_files(argc);			//check no of arguments passed from cmd line
		if( state == e_success )
		{	
			printf("INFO: Correct no. of files passed\n");
			state = read_and_validate_encode_args( argv, &encInfo);			//read file names from arguments passed and validate
			if( state == e_success )
			{	
				printf("INFO: Arguments validation successful\n");
				state = do_encoding( &encInfo );						//function call to perform encoding
				if( state == e_success )
				{
					printf("INFO: Encoding Successful\n");
					return 0;
				}
				else
				{	
					printf("ERROR: Encoding Failure\n");
					return 1;
				}
			}
			else
			{	
				printf("ERROR: Argument validation unsuccessful\n");
				printf("USAGE: Encoding ->  ./lsb_steg -e src_image.bmp secret.txt <dest_image.bmp>\n");
				return 1;
			}
		}
		else
		{
			printf("ERROR: Incorrect no. of files passed\n");
			printf("USAGE: Encoding ->  ./lsb_steg -e src_image.bmp secret.txt <dest_image.bmp>\n");
			return 1;			
		}
		fclose(encInfo.fptr_src_image);							//closing all the files opened during encoding
    	fclose(encInfo.fptr_secret);
    	fclose(encInfo.fptr_stego_image);		
	}
	else if( type == e_decode )							//decoding operation
	{	
		printf("INFO: Operation Type - Decoding\n");
		state = check_decode_files(argc);							//check for no of files passed from cmd line
		if( state == e_success )
		{
			printf("INFO: Correct no. of files passed\n");
			state = read_and_validate_decode_args( argv, &decInfo);		//read file names from arguments passed and validate
			if( state == e_success )
			{
				printf("INFO: Arguments validation successful\n");
				state = do_decoding( &decInfo );						//function call to perform decoding
				if( state == e_success )
				{
					printf("INFO: Decoding completed successfully\n");
					return 0;
				}
				else
				{
					printf("ERROR: Decoding failure\n");
					return 1;
				}			
			}
			else
			{
				printf("ERROR: Argument validation unsuccessful\n");
				printf("USAGE: Decoding ->  ./lsb_steg -d dest_image.bmp <decode.txt>\n");
				return 1;
			}
		}
		else
		{
			printf("ERROR: Incorrect no. of files passed\n");
			printf("USAGE: Decoding ->  ./lsb_steg -d dest_image.bmp <decode.txt>\n");
			return 1;
		}
    	fclose(decInfo.fptr_secret);							//closing all the files opened during decoding
    	fclose(decInfo.fptr_stego_image);						
	}	
	else	
	{	
		printf("ERROR: Operation Type - Unsupported\n");
		printf("USAGE: Encoding ->  ./lsb_steg -e src_image.bmp secret.txt <dest_image.bmp>\n");
		printf("USAGE: Decoding ->  ./lsb_steg -d dest_image.bmp <decode.txt>\n");
		return 1;
	}
	return 0;
}

