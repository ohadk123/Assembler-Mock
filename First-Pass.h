#ifndef FIRST_HEADER
#define FIRST_HEADER

#include "Global.h"

#define IC_START 100
#define DC_START 0
#define DATA ".data"
#define STRING ".string"
#define EXTERN ".extern"
#define ENTRY ".entry"

#define CHECK_TAG_NAME \
            if (!nameCheck(tagName)) \
            { \
                printf("[%d] Illegal tag name: \"%s\"\n", lineCount, tagName); \
                errors = true; \
                continue; \
            } \
            if (!tableSearch(tagName)) \
            { \
                printf("[%d] Duplicate tag name: %s\n", lineCount, tagName); \
                errors = true; \
                continue; \
            }

bool firstPass();
bool assignTag(char *name, location attrLoc, type attrType, int memo);
bool nameCheck(char *name);
char *skipWhiteSpaces(char *p);
void codeString(char *p);
void codeData(char *p);
void removeSpaces(char *str);
bool tableSearch(char *name);
int analizeCode(char *codeLine);
int identifyAddressingMode(char *operand, Instruction instruct, bool *modes, direction dir, int L);

#endif
