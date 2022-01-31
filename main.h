#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 81 /*Maximum allowed length of line*/
#define MAX_MACRO_SIZE 6*MAX_LINE /*Maximum length allowed of macro*/
#define MEM_SIZE 8192 /*Size of memory*/
#define NUM_OF_REG 15 /*Amount of registers in cpu*/

typedef enum boolean
{
	false = 0,
	true = 1
} bool;

typedef struct opcode
{
     unsigned funct : 4;
     unsigned originReg : 4;
     unsigned originType : 2;
     unsigned recieveReg : 4;
     unsigned recieveType : 2;
}opcode;

typedef struct word
{
     unsigned o : 1;
     unsigned A : 1;
     unsigned R : 1;
     unsigned E : 1;
     opcode code;
}word;

word memory[MEM_SIZE];
char **text;