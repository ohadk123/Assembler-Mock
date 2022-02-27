#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 81				 /*Maximum allowed length of line*/
#define MAX_MACRO_SIZE 6 * MAX_LINE /*Maximum length allowed of macro*/
#define MEM_SIZE 8192               /*Virtual computer's memory size*/

typedef enum boolean
{
	false = 0,
	true = 1
} bool;

enum none {none = 0};
enum location {code = 1, data = 2};
enum type {entry = 1, external = 2};

typedef union Word
{
     struct
     {
          unsigned int E :4;
          unsigned int D :4;
          unsigned int C :4;
          unsigned int B :4;
          unsigned int A :4;
     } quarter;

     struct
     {
          unsigned int destType :2;
          unsigned int destReg :4;
          unsigned int origType :2;
          unsigned int origReg :4;
          unsigned int funct :4;
          unsigned int E :1;
          unsigned int R :1;
          unsigned int A :1;
          unsigned int o :1;
     } opcode;

     unsigned short base;
     unsigned short offset;
     unsigned short number;
} Word;

typedef struct Symbol
{
     char *name;
     int value;
     int base;
     int offset;
     struct attribute
     {
          int location;
          int type;
     } attribute;
     struct Symbol *next;
} Symbol;

static Symbol firstSymbol;
static Word memory[MEM_SIZE];
static int IC, DC;

/*void printMemory()
{
    int i;
    for (i = 0; i < MEM_SIZE; i++)
    {
        if (memory[i].quarter.A != 0)
            printf("%04d\tA%d-B%d-C%d-D%d-E%d\n", i, 
                memory[i].quarter.A, memory[i].quarter.B, memory[i].quarter.C, memory[i].quarter.D, memory[i].quarter.E);
    }
}*/