#ifndef SECOND_HEADER
#define SECOND_HEADER

#include "Global.h"

bool secondPass(Word *memory, int ICF, int DCF, Symbol firstSym, Symbol *symPointer);
void codeLine(Word *memory, char *line, Symbol *symPointer, Symbol *firstSym);
bool codeEntry(Symbol *symP, Symbol *symPointer , char *name);
void printMemory();
void getAttr(int location, int type);

#endif