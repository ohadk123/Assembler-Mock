#include "Second-Pass.h"



void secondPass(Word *memory, int IC, int DC, Symbol firstSym, Symbol *symPointer, int memLoc)
{
    printMemory(memory, firstSym, symPointer, IC, DC);
    remove ("output.txt");
}

void printMemory(Word *memory, Symbol firstSym, Symbol *symPointer, int IC, int DC)
{
    int i;
    symPointer = &firstSym;
    printf("ICF = %d, DCF = %d\n", IC, DC);
    printf("NAME:\tVALUE:\tBASE:\tOFFSET:\tATTRIBUTES:\n");
    while(symPointer->next != NULL)
    {
        printf("%s\t%d\t%d\t%d\t", symPointer->name, symPointer->value, symPointer->base, symPointer->offset);
        getAttr(symPointer->attribute.location, symPointer->attribute.type);
        symPointer = symPointer->next;
    }
    for (i = 0; i < MEM_SIZE; i++)
    {
        if (memory[i].quarter.A != 0)
            printf("%04d\tA%x-B%x-C%x-D%x-E%x\n", i, 
                memory[i].quarter.A, memory[i].quarter.B, memory[i].quarter.C, memory[i].quarter.D, memory[i].quarter.E);
    }
    return;
}

void getAttr(int location, int type)
{
    bool comma = false;
    switch (location)
    {
    case(0) :
        break;
    case(1) :
        printf("code");
        comma = true;
        break;
    case(2) :
        printf("data");
        comma = true;
        break;
    }
    
    switch (type)
    {
    case(0) :
        break;
    case(1) :
        if (comma) printf(", ");
        printf("entry");
        break;
    case(2) :
        if (comma) printf(", ");
        printf("extern");
        break;
    }
    printf("\n");
    return;
}

