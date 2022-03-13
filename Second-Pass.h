#ifndef SECOND_HEADER
#define SECOND_HEADER

#include "Global.h"

bool secondPass(Word *memory, int ICF, int DCF, Label firstLabel, Label *symPointer);
void codeLine(Word *memory, char *line, Label *symPointer, Label *firstLabel);
bool codeEntry(Label *symP, Label *symPointer , char *name);
void printMemory();
void getAttr(int location, int type);

#endif