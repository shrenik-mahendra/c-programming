typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname; //Store argv[2] == src_image_fname = argv[2];
	
	
    FILE *fptr_src_image; //fptr_src_image = fopen(src_image_fname, "r");
	
	
    uint image_capacity; // store size of image file
	
	uint bits_per_pixel; 
	
    char image_data[MAX_IMAGE_BUF_SIZE];  //image_data[8]

    
	/* Secret File Info */
    char *secret_fname;//Store argv[3] == secret_fname = argv[3];
	
    FILE *fptr_secret;//fptr_secret = fopen(secret_fname, "r");
	
    char extn_secret_file[MAX_FILE_SUFFIX]; // extn_secret_file[4] = ".txt"
	
    char secret_data[MAX_SECRET_BUF_SIZE]; //secret_data[1] = byte by byte
	
    long size_secret_file; //store secret file size

   
   /* Stego Image Info */
    char *stego_image_fname; //store argv[4] if given 
							 // if not need to store "stego.bmp"
								
    FILE *fptr_stego_image; //fptr_stego_image = fopen(stego_image_fname, "w");

} EncodeInfo;


1. Check Operation Type
	
 ==> Prototype   ==>  OperationType check_operation_type(char *argv[]);
 ==> How to call ==>  type = check_operation_type(argv);
 ==> Fn Defn     ==> 
OperationType check_operation_type(char *argv[])
{
	1. U need to check weather argv[1] is "-e" or "-d" or something else
	2. if argv[1] is "-e"
		return e_encode;
	   else if argv[1] is "-d"
		return e_decode;
	   else	
		return e_unsupported:
}	

2. Read and validate arguememnts

 ==> Prototype  ==> Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);
 ==> how to call => state = read_and_validate_encode_args(argv, &encInfo);
 ==> Fn Defn     ==> 
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	1. U need to check weather argv[2] is a .bmp file or not	
		if it is .bmp u need to assign name to src_image_fname
		src_image_fname = argv[2];
		else
			return e_failure;
	2. U need to check weather argv[3] is a .txt file or not	
		if it is .txt u need to assign name to secret_fname
		secret_fname = argv[3];
		else
			return e_failure;
	3. U need to check weather argv[4] is given or not
		if given we need to atore that name in stego_image_fname
			stego_image_fname = argv[4]
			return e_success;
		else
			stego_image_fname = "stego.bmp"
			return e_success;
}

3. Do encodeing
 ==> Prototype ==> Status do_encoding(EncodeInfo *encInfo);
 ==> fn call   ==> state = do_encoding(&encInfo);
 ==> fn defn   ==>
Status do_encoding(EncodeInfo *encInfo)
{
	1. Status open_files(EncodeInfo *encInfo); == Call this first
	if (open_files(encInfo) == e_success)
	{
		printf("Info : Open files Success\n");
		if (check_capacity(encInfo) == e_success)
		{
			printf("Info: Check capacity success\n");
			2. Copy BMP Header
			if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
			{
				printf("Info: Copy BMP header success\n");
				if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf("INFO: Magic string encode is success\n");
					//Extention size
					if (encode_size((strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success
					{
						printf("INFO: Entx size encode is success\n");
						if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
						{
							PRINT SUCCESS;
							if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
							{
								PRINT SUCCESS;
								if(encode_secret_file_data(encInfo) == e_success)
								{
									PRINT SUCCESS;
									if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
									{
										PRINT
										return e_success;
									}
									else
									{
										PRINT ERROR;
										return e_failure;
									}									
								}
								else
								{
									PRINT FAILURE;
									return e_failure;
								}
							}
							else
							{
								PRINT FAILURE;
								return e_failure;
							}							
						}
						else
						{
							PRINT FAILURE;
							return e_failure;
						}
					}
					else
					{
						printf("ERROR: Extn size encode is failure\n");
						return e_failure;
					}
				}
				else
				{
					printf("ERROR: Magic string encode is failure\n");
					return e_failure;
				}
			}
			else
			{
				printf("Error: Copy BMP header failure\n");
				return e_failure;
			}
		}
		else
		{
			printf("Error : Cannot do encodeing because capacity is less\n");
			return e_failure;
	}
	else
	{
		printf("Error: Open files Failure\n");
		return e_failure;
	}
}

4. Open files
 ==> Prototype ==> Status open_files(EncodeInfo *encInfo)
 ==> fn call   ==> state = open_files(encInfo);
 ==> fn defn   ==>
Status open_files(EncodeInfo *encInfo)
{
	Already Written
}

5. Check capacity

==> Prototype ==> Status check_capacity(EncodeInfo *encInfo);
 ==> fn call   ==> state = check_capacity(encInfo);
 ==> fn defn   ==>
Status check_capacity(EncodeInfo *encInfo)
{
	1. U need to get the image size
	uint get_image_size_for_bmp(FILE *fptr_image); // Already written
	
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

	/* Get file size */
	//uint get_file_size(FILE *fptr);
	
DO  ==	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	
	DO the comparision 
	if (encInfo->image_capacity >= 54 + 32 + 32 + 32 + size_secret_file * 8)
		return e_success
	else
		return e_failure;
	
}

6. Get secret file size
 ==> prototype ==> uint get_file_size(FILE *fptr);
 ==> fn call ===> get_file_size(encInfo->fptr_secret);
 ==> fn definition ==>
get_file_size(encInfo->fptr_secret)
{
	1. Use fseek and ftell, rewind function
		move the file pointer to last using fseek
		use ftell to get the offset position which is size of file
		use rewind to make file pointer points to beginnig of file
		return ftell_value;
}

7. Copy BMP header
==> prototype => Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image);
==> fn call => copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)
==> fn definition
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image)
{
	//Need to get 54 bytes from fptr_src_image
	//Copy to fptr_dest_image
	return e_success;
}

8. Encode magic string
==> prototype => Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);
==> fn call => encode_magic_string(MAGIC_STRING , encInfo);
==> fn definition
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	// Encode "#*" here
	1. How many bytes we need to encode == #* == 2bytes
	//CALL encode_data_to_image function
	encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
	return e_success;
}	

9. Encode data to image
==>prototype
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);
==>fn call 
encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
==> fn definition
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	size  ====  2 #*, 4 for .txt
	// To encode 2byte of information i need 2 * 8 = 16 bytes.
	for (size times 2 times 1 for # another for *) // .txt 4time
	{
		//read 8 byte from fptr_src_image (orig.bmp)store it in buffer == image_data[MAX_IMAGE_BUF_SIZE]
		//we took 8bytes from original.bmp then we need to encode # to it
		//encode # to the buffer 
		//to do that one we have a function Status encode_byte_tolsb(char data, char *image_buffer);
		fread(buffer, , ,fptr_src_image);
		encode_byte_tolsb(data[i], buffer);
		fwrite(buffer, , ,fptr_stego_image);
		// USE ftell for your reference
		62
	}
	//70 file pointers
	return e_success;
}

10. Encode byte to LSB
==>prototype == > Status encode_byte_tolsb(char data, char *image_buffer);
==>fn call ==> encode_byte_tolsb(data[i], buffer);
==> fn definition
Status encode_byte_tolsb(char data, char *image_buffer)
{
	data # == 0x23 == 0010 0011
	8 bytes array
	
	0 we need to put in image_buffer[0] lsb bit
	0 we need to put in image_buffer[1] lsb bit
	1 we need to put in image_buffer[2] lsb bit
	0 we need to put in image_buffer[3] lsb bit
	0 we need to put in image_buffer[4] lsb bit
	0 we need to put in image_buffer[5] lsb bit
	1 we need to put in image_buffer[6] lsb bit
	1 we need to put in image_buffer[7] lsb bit
	
	for (8 times for 8 bits)
	{
		get bits from data from msb
		retain 7bits in image_buffer[i] except LSB
		stote get bit at LSB of image_buffer[i]
	}	
	return e_success;
}	

11. Encode size of extn or size of file
==>prototype ==> Status encode_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image);
== fn call ==> encode_size(strlen(extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image);
==> fn definition
Status encode_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char buffer[32];
	//fread == 32 bytes from  fptr_src_image store to buffer
	fread(buffer, , ,fptr_src_image);
	encode_size_to_LSB(size, buffer);
	fwrite (buffer, , , fptr_steg_image);
	return e_success;
	
}
/* Encode size to lsb */
Status encode_size_to_LSB(int size, char *buffer)
{
	for (32 times for 32 bits because size is int)
	{
		get bits from size from msb
		retain 7bits in image_buffer[i] except LSB
		stote get bit at LSB of image_buffer[i]
	}	
	return e_success;
}
	
12. Encode secret file extention ".txt"
==> prototype ==> Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);
==> fn call ==> encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
==> fn definition
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
	return e_success;
}


13. Encode secret file size 
==> prototype ==> Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);
==> fn call ==>  encode_secret_file_size(encInfo->size_secret_file, encInfo);
==> fn definiton
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	encode_size(file_size, encInfo->fptr_src_image, encInfo->fptr_steg_image);
	return e_success;
}

14. Secret file data
==> prototype ==> Status encode_secret_file_data(EncodeInfo *encInfo);
==> fn call ==> encode_secret_file_data(encInfo);
==> fn definition
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	==> We need to encode size_secret_file bytes
	char ch, buffer[8];
	for (size_secret_file)
	{
		//read 1 byte from secret file.
		//read 8 bytes from sourece image original.bmp
		fread(&ch, , , encInfo->fptr_secret);
		fread(buf, , , encInfo->fptr_src_image);
		encode_byte_tolsb(ch, buffer);
		fwrite(buffer, , , encInfo->fptr_stego_image);
	}
	return e_success;
}

15. Copy Reamaining Bytes
==prototype ==> Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);
==>fn call ==> copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
==> fn definition
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	READ FROM fptr_src
	WRITE TO fptr_dest
	while ()
	{
		
	}
	return e_success;
}

	