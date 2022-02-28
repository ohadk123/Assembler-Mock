#include "First-Pass.h"

Symbol firstSym = {"", 0, 0, 0, {0, 0}, NULL};
Symbol *symPointer = &firstSym;
int memLoc = 100;

int firstPass()
{
    bool errors = false;
    FILE *text;
    char line[MAX_LINE];
    char *startP;
    char *endP;
    char *tagName = (char *)malloc(sizeof(char));
    int lineCount = 0;
    bool symFlag = false;
    int i;

    text = fopen("output.txt", "r");
    if (text == NULL)
     {
          fprintf(stderr, "Error trying to open output file, stupid");
          fprintf(stderr, "The program will continue to the next file\n");
          return -1;
     }
     fseek(text, 0, SEEK_SET);
    
    while (fgets(line, MAX_LINE, text) != NULL)
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
                errors = true;
                continue;
            }

            startP = endP+1;
        }
        
        startP = skipWhiteSpace(startP);
        

        if(isData(startP))
        {
            if (symFlag)
            {
                if (!tableSearch(&firstSym, tagName))
                {
                    fprintf(stderr, "[%d] Duplicate tag name: %s\n", lineCount, tagName);
                    errors = true;
                    continue;
                }
                symPointer->name = (char *)calloc(strlen(tagName), sizeof(char));
                strcpy(symPointer->name, tagName);
                symPointer->value = memLoc;
                symPointer->base = memLoc/16;
                symPointer->offset = memLoc%16;
                symPointer->attribute.location = data;
                symPointer->next = (Symbol *)malloc(sizeof(Symbol));
                symPointer = symPointer->next;
                symFlag = false;
            }
            
            if (!strncmp(startP, ".data", 5))
            {
                removeSpaces(startP);
                codeData(startP + 5);
            }
            else
            {
                codeString(startP + 9);
            }
            continue;
        }

        if (!strncmp(startP, ".entry", 6))
            continue;
        
        if (!strncmp(startP, ".extern", 7))
        {
            tagName = (char *)malloc(sizeof(endP+2));
            strcpy(tagName, endP+2);
            if (tagName[strlen(tagName)-1] == '\n')
                tagName[strlen(tagName)-1] = '\0';

            if (!nameCheck(tagName))
            {
                fprintf(stderr, "[%d] Illegal tag name: %s\n", lineCount, tagName);
                errors = true;
                continue;
            }

            if (!tableSearch(&firstSym, tagName))
            {
                fprintf(stderr, "[%d] Duplicate tag name: %s\n", lineCount, tagName);
                errors = true;
                continue;
            }

            symPointer->name = (char *)calloc(strlen(tagName), sizeof(char));
            strcpy(symPointer->name, tagName);
            symPointer->value = 0;
            symPointer->base = 0;
            symPointer->offset = 0;
            symPointer->attribute.type = external;
            symPointer->next = (Symbol *)malloc(sizeof(Symbol));
            symPointer = symPointer->next;
            symFlag = false;
            continue;
        }
        
        if (symFlag)
        {
            if (!tableSearch(&firstSym, tagName))
            {
                fprintf(stderr, "[%d] Duplicate tag name: %s\n", lineCount, tagName);
                errors = true;
                continue;
            }
            symPointer->name = (char *)calloc(strlen(tagName), sizeof(char));
            strcpy(symPointer->name, tagName);
            symPointer->value = memLoc;
            symPointer->base = memLoc/16;
            symPointer->offset = memLoc%16;
            symPointer->attribute.location = code;
            symPointer->next = (Symbol *)malloc(sizeof(Symbol));
            symPointer = symPointer->next;
            symFlag = false;
        }
            
        if (nameCheck(startP))
        {
            fprintf(stderr, "[%d] Operation name does not exist! %s\n", lineCount, startP);
            errors = true;
            continue;
        }

        analizeCodeLine(startP);
    }
    
    symPointer = &firstSym;
    printf("NAME:\tVALUE:\tBASE:\tOFFSET:\tATTRIBUTES:\n");
    while(symPointer->next != NULL)
    {
        printf("%s\t%d\t%d\t%d\t", symPointer->name, symPointer->value, symPointer->base, symPointer->offset);
        getAttr(symPointer->attribute.location, symPointer->attribute.type);
        symPointer = symPointer->next;
    }
    printMemory();
    
    fclose(text);
    return errors;
}

bool nameCheck(char *name)
{
    char *names[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    int i;
    for (i = 0; names[i]; i++)
    {
        if (strlen(name) >= strlen(names[i]) && !strncmp(name, names[i], strlen(names[i])))
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

void codeString(char *p)
{
    while(*p)
    {
        if (*p == 34)
            break;
        memory[memLoc].opcode.A = 1;
        memory[memLoc].number = *p;
        memLoc++;
        DC++;
        p++;
    }
    memory[memLoc].opcode.A = 1;
    memory[memLoc].number = 0;
    memLoc++;
}

void codeData(char *p)
{
    int num, i;
    while (true)
    {
        num = atof(p);
        memory[memLoc].opcode.A = 1;
        memory[memLoc].number = num;
        memLoc++;
        DC++;
        for (i = 0; p[i] != ',' && p[i] != '\n'; i++);
        p += i;
        switch (*p)
        {
        case(',') :
            p++;
            break;
        case ('\n') :
            return;
        }
    }
}

void removeSpaces(char *str)
{
    int i, j;
     for (i = 0, j = 0; str[i]; i++)
     {
          if (str[i] != ' ')
               str[j++] = str[i];
     }
     str[j] = '\0';
}

bool tableSearch(Symbol *symP, char *name)
{
    while (symP != NULL)
    {
        if (!strcmp(symP->name, name))
            return false;
        symP = symP->next;
    }
    return true;
}

void analizeCodeLine(char *line)
{
    
}

void printMemory()
{
    int i;
    for (i = 0; i < MEM_SIZE; i++)
    {
        if (memory[i].quarter.A != 0)
            printf("%04d\tA%x-B%x-C%x-D%x-E%x\n", i, 
                memory[i].quarter.A, memory[i].quarter.B, memory[i].quarter.C, memory[i].quarter.D, memory[i].quarter.E);
    }
}