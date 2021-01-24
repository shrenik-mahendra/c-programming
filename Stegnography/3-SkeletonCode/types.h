#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in function return type */
typedef enum
{
    e_success,			//e_success = 0
    e_failure			//e_faliure = 1
} Status;

/* OperationType will be used in function return type */
typedef enum
{
    e_encode,			//e_encode = 0
    e_decode,			//e_decode = 1
    e_unsupported		//e_unsupported = 2
} OperationType;

#endif


