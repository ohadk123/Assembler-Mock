#include "Global.h"

int firstPass();
bool nameCheck(char *name);
bool isData(char *p);
char *skipWhiteSpace(char *p);
void getAttr(int location, int type);
void codeString(char *p);
void codeData(char *p);
void removeSpaces(char *str);
bool tableSearch(Symbol *symP, char *name);
bool insertSym(Symbol *symP, char *name, int value);
void printMemory();