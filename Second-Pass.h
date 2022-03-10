#ifndef SECOND_HEADER
#define SECOND_HEADER

#include "Global.h"

void secondPass(Word *memory, int IC, int DC, Symbol firstSym, Symbol *symPointer, int memLoc);
void printMemory();
void getAttr(int location, int type);

#endif