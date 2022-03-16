#ifndef FIRST_HEADER
#define FIRST_HEADER

#include "Global.h"

#define IC_START 100
#define DC_START 0
#define DATA ".data"
#define STRING ".string"
#define EXTERN ".extern"
#define ENTRY ".entry"

#define CHECK_LABEL_NAME \
            if (!nameCheck(labelName)) \
            { \
                printf("[%d] Illegal label name: \"%s\"\n", lineCount, labelName); \
                firstErrors = true; \
                continue; \
            } \
            if (!tableSearch(labelName)) \
            { \
                printf("[%d] Duplicate label name: %s\n", lineCount, labelName); \
                firstErrors = true; \
                continue; \
            }

bool firstPass();
bool assignLabel(char *name, location attrLoc, type attrType, int memo);
bool nameCheck(char *name);
char *skipWhiteSpaces(char *p);
void codeString(char *p);
void codeData(char *p);
void removeSpaces(char *str);
bool tableSearch(char *name);
int analizeCode(char *codeLine);
int identifyAddressingMode(char *operand, Instruction instruct, bool *modes, direction dir, int L);

#endif
