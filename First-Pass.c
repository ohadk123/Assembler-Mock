#include "First-Pass.h"

Symbol firstSym = {"", 0, 0, 0, {0, 0}, NULL};
Symbol *symPointer = &firstSym;
int memLoc = 100;

int firstPass()
{
    FILE *code;
    char line[MAX_LINE];
    char *startP;
    char *endP;
    char *tagName = (char *)malloc(sizeof(char));
    int lineCount = 0;
    bool symFlag = false;
    int i;

    code = fopen("output.txt", "r");
    if (code == NULL)
     {
          fprintf(stderr, "Error trying to open output file, stupid");
          fprintf(stderr, "The program will continue to the next file\n");
          return -1;
     }
     fseek(code, 0, SEEK_SET);
    
    while (fgets(line, MAX_LINE, code) != NULL)
    {
        tagName = (char *)calloc(1, sizeof(char));
        lineCount++;
        startP = line;
        endP = line;
        while (endP[1] != ' ')
            endP++;
        if (*endP == ':')
        {
            symFlag = true;
            tagName = (char *)calloc((endP-startP), sizeof(char));
            for (i = 0; line[i] != ':'; i++)
                tagName[i] = line[i];
            
            if (!nameCheck(tagName))
            {
                fprintf(stderr, "[%d] Illegal tag name: %s\n", lineCount, tagName);
                continue;
            }

            startP = endP+1;
        }
        
        startP = skipWhiteSpace(startP);
        
        if(isData(startP) && symFlag)
        {
            if (symFlag)
            {
                symPointer->name = (char *)calloc(strlen(tagName), sizeof(char));
                strcpy(symPointer->name, tagName);
                symPointer->value = memLoc;
                symPointer->base = memLoc/16;
                symPointer->offset = memLoc%16;
                symPointer->attribute.location = data;
                symPointer->next = (Symbol *)malloc(sizeof(Symbol));
                symPointer = symPointer->next;
                memLoc++;
                symFlag = false;
            }
            (!strncmp(startP, ".data", 5)) ? (codeData(startP + 6)) : (codeData(startP + 9));
            continue;
        }
        memLoc++;
        symFlag = false;
    }
    /*
    symPointer = &firstSym;
    printf("NAME:\tVALUE:\tBASE:\tOFFSET:\tATTRIBUTES:\n");
    while(symPointer->next != NULL)
    {
        printf("%s\t%d\t%d\t%d\t", symPointer->name, symPointer->value, symPointer->base, symPointer->offset);
        getAttr(symPointer->attribute.location, symPointer->attribute.type);
        symPointer = symPointer->next;
    }
    */
    fclose(code);
    printMemory();
    return 1;
}

bool nameCheck(char *name)
{
    char *names[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    int i;
    
    for (i = 0; names[i]; i++)
    {
        if (strlen(name) == strlen(names[i]) && !strncmp(name, names[i], strlen(names[i])))
            return false;
    }
    return true;
}

bool isData(char *p)
{
    if (!strncmp(p, ".data", 5) || !strncmp(p, ".string", 7))
        return true;
    return false;
}

char *skipWhiteSpace(char *p)
{
     int i;
     for (i = 0; p[i] != 0 && (p[i] == ' ' || p[i] == '\t'); i++); /*Count how many white spaces are there*/
     return p + i;
}

void getAttr(int location, int type)
{
    char *attr = "";
    switch(location)
    {
        case(0) :
            break;
        case(1) :
            attr = (char *)calloc(6, sizeof(char *));
            strcat(attr, "code ,");
            break;
        case(2) :
            attr = (char *)calloc(6, sizeof(char *));
            strcat(attr, "data ,");
            break;
    }

    switch(type)
    {
        case(0) :
            break;
        case(1) :
            attr = (char *)malloc(sizeof(attr) + 5*sizeof(char *));
            strcat(attr, "entry");
            break;
        case(2) :
            attr = (char *)malloc(sizeof(attr) + 8*sizeof(char *));
            strcat(attr, "external");
            break;
    }
    printf("%s\n", attr);
    return;
}

void codeData(char *p)
{
    while(*p)
    {
        if (*p == 34)
        {
            memory[memLoc].opcode.A = 1;
            memLoc++;
            return;
        }
        memory[memLoc].opcode.A = 1;
        (isdigit(*p)) ? (memory[memLoc].number = (*p-'0')) : (memory[memLoc].number = *p);
        memLoc++;
        p++;
    }
}

void printMemory()
{
    int i;
    for (i = 0; i < MEM_SIZE; i++)
    {
        if (memory[i].quarter.A != 0)
            printf("%04d\tA%d-B%d-C%d-D%d-E%d\n", i, 
                memory[i].quarter.A, memory[i].quarter.B, memory[i].quarter.C, memory[i].quarter.D, memory[i].quarter.E);
    }
}