#ifndef PRE_HEADER
#define PRE_HEADER

#include "Global.h"

bool preAssembler(char *argv);
int readMacro(char p[], int m, FILE *fp);
void unfoldMacro(int m);
char *skipWhiteSpace(char *p);

#endif