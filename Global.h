#ifndef GLOBAL_HEADER
#define GLOBAL_HEADER 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LINE 81				 /*Maximum allowed length of line*/
#define MAX_MACRO_SIZE 6 * MAX_LINE /*Maximum length allowed of macro*/
#define MEM_SIZE 8192               /*Virtual computer's memory size*/

typedef enum boolean
{
	false = 0,
	true = 1
} bool;

enum none {none = 0};
typedef enum location {code = 1, data = 2} location;
typedef enum type {entry = 1, external = 2} type;

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
          unsigned int destMode :2;
          unsigned int destReg :4;
          unsigned int origMode :2;
          unsigned int origReg :4;
          unsigned int funct :4;
          unsigned int E :1;
          unsigned int R :1;
          unsigned int A :1;
          unsigned int o :1;
     } opcode;

     short base;
     short offset;
     short number;
} Word;

typedef struct Label
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
     struct Label *next;
} Label;

enum addressingModes {immediate, direct, index, regDirect};
typedef enum direction {origin, destination} direction;

typedef struct Instruction
{
     char *name;
     int opcode;
     int funct;
     int numOfOps;
     /* {immidiate, direct, index, regDirect} */
     bool origModes[4];
     bool destModes[4];
} Instruction;

#endif