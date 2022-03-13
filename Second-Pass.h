#ifndef SECOND_HEADER
#define SECOND_HEADER

#include "Global.h"

void secondPass(Word *memory, int ICF, int DCF, Label firstLabel, Label *symPointer, char *fileName);
void codeLine(Word *memory, char *line, Label *symPointer, Label *firstLabel);
bool getLabel(Word *memory, Label *labelPointer, Label *labelP, char *name);
bool codeEntry(Label *symP, Label *symPointer , char *name);
void outputFile(Word *memory, Label *labelP, Label *labelPointer, int IC, int DC, char *fileName);
void getAttr(int location, int type);

#endif