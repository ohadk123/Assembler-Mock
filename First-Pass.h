#ifndef FIRST_HEADER
#define FIRST_HEADER

#include "Global.h"

#define IC_START 100
#define DC_START 0

#define CHECK_TAG_NAME \
            if (!nameCheck(tagName)) \
            { \
                printf("Illegal tag name: \"%s\"\n", tagName); \
                errors = true; \
                continue; \
            } \
            if (!tableSearch(tagName)) \
            { \
                printf("Duplicate tag name: %s\n",tagName); \
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
int analizeCode(char *codeLine);
int identifyAddressingMode(char *operand, Instruction instruct, bool *modes, direction dir);
void printMemory();

#endif
