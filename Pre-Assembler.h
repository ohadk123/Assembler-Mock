#ifndef PRE_HEADER
#define PRE_HEADER

#include "Global.h"

int preAssembler(char *fileName);
int runMacro(char *argv);
void readMacro(char p[], int m, FILE *fp);
int unfoldMacro(int i, int m);
char *skipWhiteSpace(char *p);
#define SKIP_WHITE

#endif