#include "First-Pass.h"

Word memory[MEM_SIZE];
int IC = IC_START, DC = DC_START;
Symbol firstSym = {"", 0, 0, 0, {0, 0}, NULL};
Symbol *symPointer = &firstSym;
int memLoc = IC_START;
int lineCount = 0;
int L;                                          /* {immid, direc, index, reg}    {immid, direc, index, reg} */
Instruction instructions[] = {{"mov", 0.0, 0, 2,   {true,  true,  true,  true},  {false, true,  true,  true}}, 
                              {"cmp", 1.0, 0, 2,   {true,  true,  true,  true},  {true,  true,  true,  true}},
                              {"add", 2.0, 10, 2,  {true,  true,  true,  true},  {false, true,  true,  true}},
                              {"sub", 2.0, 11, 2,  {true,  true,  true,  true},  {false, true,  true,  true}},
                              {"lea", 4.0, 0, 2,   {false, true,  true,  false}, {false, true,  true,  true}},
                              {"clr", 5.0, 10, 1,  {false, false, false, false}, {false, true,  true,  true}},
                              {"not", 5.0, 11, 1,  {false, false, false, false}, {false, true,  true,  true}},
                              {"inc", 5.0, 12,1 ,  {false, false, false, false}, {false, true,  true,  true}},
                              {"dec", 5.0, 13, 1,  {false, false, false, false}, {false, true,  true,  true}},
                              {"jmp", 9.0, 10, 1,  {false, false, false, false}, {false, true,  true,  false}}, 
                              {"bne", 9.0, 11, 1,  {false, false, false, false}, {false, true,  true,  false}},
                              {"jsr", 9.0, 12, 1,  {false, false, false, false}, {false, true,  true,  false}},
                              {"red", 12.0, 0, 1,  {false, false, false, false}, {false, true,  true,  true}},
                              {"prn", 13.0, 0, 1,  {false, false, false, false}, {true,  true,  true,  true}},
                              {"rts", 14.0, 0, 0,  {false, false, false, false}, {false, false, false, false}},
                              {"stop", 15.0, 0, 0, {false, false, false, false}, {false, false, false, false}}};

int firstPass()
{
    FILE *text;
    char line[MAX_LINE];
    char *startP;
    char *endP;
    char *tagName = (char *)malloc(sizeof(char *));
    bool symFlag = false;
    int i;
    bool errors = false;

    /* Opening the processed code */ {
    text = fopen("output.txt", "r");
    if (text == NULL)
     {
          fprintf(stderr, "Error trying to open output file, stupid");
          fprintf(stderr, "The program will continue to the next file\n");
          return -1;
     }
     fseek(text, 0, SEEK_SET);
    }

    while (fgets(line, MAX_LINE, text) != NULL)
    {
        if (!strcmp(line, "\n"))
        {
            lineCount++;
            continue;
        }
        tagName = " ";
        lineCount++;
        startP = line;
        endP = line;
        symFlag = false;

        while (endP[1] != ' ')
            endP++;

        /* Locating tags */
        if (*endP == ':')
        {
            symFlag = true;
            tagName = (char *)calloc((endP-startP), sizeof(char));
            for (i = 0; line[i] != ':'; i++)
                tagName[i] = line[i];
            
            /*CHECK_NAME(tagName)*/

            startP = ++endP;
        }

        startP = skipWhiteSpaces(startP);

        /* Handling data encoding */
        if (isData(startP))
        {
            if (symFlag)
            {
                symPointer->name = (char *)malloc(sizeof(tagName));
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

        /* Entry type handled in second pass */
        if (!strncmp(startP, ".entry", 6))
            continue;

        /* Handling extern type */
        if (!strncmp(startP, ".extern", 7))
        {
            tagName = (char *)malloc(sizeof(endP+2));
            strcpy(tagName, endP+2);
            if (tagName[strlen(tagName)-1] == '\n')
                tagName[strlen(tagName)-1] = '\0';
            
            /*CHECK_NAME(tagName)*/

            symPointer->name = (char *)malloc(sizeof(tagName));
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
        	symPointer->name = (char *)malloc(sizeof(tagName));
            strcpy(symPointer->name, tagName);
            symPointer->value = memLoc;
            symPointer->base = memLoc/16;
            symPointer->offset = memLoc%16;
            symPointer->attribute.location = code;
            symPointer->next = (Symbol *)malloc(sizeof(Symbol));
            symPointer = symPointer->next;
            symFlag = false;
        }
        
        L = analizeCode(startP);
        if (L == 0)
            errors = true;
        IC += L;
        memLoc += L;
    }

    free(tagName);
    fclose(text);
    printMemory();
    return errors;
}

bool nameCheck(char *name)
{
    char *names[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    int i;
    for (i = 0; names[i]; i++)
    {
        if (strlen(name) >= strlen(names[i]) && !strcmp(name, names[i]))
            return false;
    }

    if (!isalpha(*name))
        return false;

    for (i = 1; i < strlen(name); i++)
    {
        if (!isalnum(name[i]))
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

char *skipWhiteSpaces(char *p)
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
    DC++;
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

bool tableSearch(char *name)
{
    Symbol *symP = &firstSym;
    while (symP->next != NULL)
    {
        if (!strcmp(symP->name, name))
            return false;
        symP = symP->next;
    }
    return true;
}

int analizeCode(char *codeLine)
{
    int size = sizeof(instructions)/sizeof(Instruction);
    char *firstOp, *secondOp;
    char ct[2] = ",";
	int i, j, L = 1, l;
    Instruction *instruct;
    direction dir;
	
    for (i = 0; i < size; i++)
    {
        instruct = instructions + i;
        if (strlen(codeLine) >= strlen(instruct->name) && !strncmp(codeLine, instruct->name, strlen(instruct->name)))
        {
            for (j = 0; codeLine[j] != ' ' && codeLine[j] != '\t' && codeLine[j] != '\n'; j++); /* Skips instruction */
            codeLine = skipWhiteSpaces(codeLine+j); /* Ignoring white space after instruction */
            codeLine[strlen(codeLine)-1] = '\0'; /* Removing \n from the end for convenience */
            firstOp = strtok(codeLine, ct); /* First operand */
            secondOp = strtok(NULL, ct); /* Second operand */

            /* There never is a third operand, error if there is */
            if (strtok(NULL, ct) != NULL) 
            {
                printf("[%d] Too many operands!\n", lineCount);
                return 0;
            }

            /* If no operands needed, error if first operand exists
             * Zero operand instructions use only 1 word */
            if (instruct->numOfOps == 0)
            {
                if (firstOp != NULL)
                {
                    printf("[%d] Extraneous text after instruction!\n", lineCount);
                    return 0;
                }
                    return L;
            }
            
            L++; /*Funct word */

            /* If only one operand needed, error occures if even second operand exists */
            if (instruct->numOfOps == 1 && secondOp != NULL)
            {
                printf("[%d] Too many operands!\n", lineCount);
                return 0;
            }

            dir = instruct->numOfOps == 1 ? destination : origin;

            l = identifyAddressingMode(firstOp, *instruct, dir ? instruct->destModes : instruct->origModes, dir);
            if (l == -1)
                return 0;
            L += l;
            if (instruct->numOfOps == 1) /* If only one operand needed we are done here */
                return L;

            secondOp = skipWhiteSpaces(secondOp);
            l = identifyAddressingMode(secondOp, *instruct, instruct->destModes, destination);
            if (l == -1)
                return 0;
            L+= l;
            
            return L;
        }
    }
    /* Looped through entire instruction list and not found a correct instruction */
	printf("[%d] Unknown intsruction!", lineCount);
    return 0;
}

int identifyAddressingMode(char *operand, Instruction instruct, bool *modes, direction dir)
{
    #define DIRECTION(dir) dir ? "destination" : "origin"
    int j = 0;
    bool isTag = false;
    int number;

    /* Checks for immediate addressing mode
     * checks if it's actually a number, skips '-' if it exists
     * Error if there is a char which is not a digit
     * Immidiate addressing mode uses only 1 word */
    if (*operand == '#')
    {   
        for ((operand[j] == '-') ? (j = 1) : (j = 2); j < strlen(operand); j++)
        {
            if (!isdigit(operand[j]))
            {
                printf("[%d] A whole number must follow up a # prefix!\n", lineCount);
                return -1;
            }
        }
        if (modes[immediate])
            return 1;
        printf("[%d] instruction doesnt support immidiate addressing mode for %s operand\n", lineCount, DIRECTION(dir));
        return -1;
    }
    /* Check if operand is a register or a tag
     * Register name is only a whole number, might be a tag if a char is not a digit
     * Error if the char is not a letter as well
     * If all characters are digits, it might be a register or a tag 
     * Register names are in range 0-15, else might be a tag */
    else if (*operand == 'r')
    {
        for (j = 1; j < strlen(operand); j++)
        {
            if (!isdigit(operand[j]))
            {
                if (!isalpha(operand[j]))
                {
                    printf("[%d] Illegal character in tag name!", lineCount);
                    return -1;
                }
                isTag = true;
            }
        }
        if (!isTag)
        {
            number = atoi(operand+1);
            if (number > 15)
                isTag = true;
            else
            {
                if (modes[regDirect])
                    return 0;
                printf("[%d] instruction doesnt support register addressing mode for %s operand\n", lineCount, DIRECTION(dir));
                return -1;
            }
        }
    }
    if (modes[direct])
        return 2;
    printf("[%d] instruction doesnt support tag type addressing mode for %s operand\n", lineCount, DIRECTION(dir));
    return -1;
}

void printMemory()
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
