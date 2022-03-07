#ifndef FIRST_HEADER
#define FIRST_HEADER

#include "Global.h"

#define IC_START 100
#define DC_START 0

#define CHECK_NAME(name) \
            if (!nameCheck(name)) \
            { \
                printf("Illegal tag name: \"%s\"\n", name); \
                errors = true; \
                continue; \
            } \
            if (!tableSearch(name)) \
            { \
                printf("Duplicate tag name: %s\n",name); \
                errors = true; \
                continue; \
            }

int firstPass();
bool nameCheck(char *name);
bool isData(char *p);
char *skipWhiteSpaces(char *p);
void getAttr(int location, int type);
void codeString(char *p);
void codeData(char *p);
void removeSpaces(char *str);
bool tableSearch(char *name);
void analizeCode(char *codeLine);
int codeTwoOp(char *codeLine, int instruction);
int codeOneOp(char *codeLine, int instruction);
void printMemory();

#endif
