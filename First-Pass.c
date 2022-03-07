#include "First-Pass.h"

Word memory[MEM_SIZE];
int IC = IC_START, DC = DC_START;
Symbol firstSym = {"", 0, 0, 0, {0, 0}, NULL};
Symbol *symPointer = &firstSym;
int lineCount = 0;
bool errors = false;
Instruction instructions[] = {{"mov", 0.0, 0, 2}, {"cmp", 1.0, 0, 2}, {"add", 2.0, 10, 2}, {"sub", 2.0, 11, 2}, {"lea", 4.0, 0, 2},
                              {"clr", 5.0, 10, 1},{"not", 5.0, 11, 1}, {"inc", 5.0, 12,1 }, {"dec", 5.0, 13, 1}, {"jmp", 9.0, 10, 1}, 
                              {"bne", 9.0, 11, 1}, {"jsr", 9.0, 12, 1}, {"red", 12.0, 0, 1}, {"prn", 13.0, 0, 1},
                              {"rts", 14.0, 0, 0}, {"stop", 15.0, 0, 0}};

int firstPass()
{
    FILE *text;
    char line[MAX_LINE];
    char *startP;
    char *endP;
    char *tagName = (char *)malloc(sizeof(char *));
    bool symFlag = false;
    int i;

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
                symPointer->value = IC;
                symPointer->base = (IC/16)*16;
                symPointer->offset = IC%16;
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
            symPointer->value = IC;
            symPointer->base = (IC/16)*16;
            symPointer->offset = IC%16;
            symPointer->attribute.location = code;
            symPointer->next = (Symbol *)malloc(sizeof(Symbol));
            symPointer = symPointer->next;
            symFlag = false;
        }
        
        IC += analizeCode(startP);
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
        memory[IC].opcode.A = 1;
        memory[IC].number = *p;
        IC++;
        DC++;
        p++;
    }
    memory[IC].opcode.A = 1;
    memory[IC].number = 0;
    IC++;
}

void codeData(char *p)
{
    int num, i;
    while (true)
    {
        num = atof(p);
        memory[IC].opcode.A = 1;
        memory[IC].number = num;
        IC++;
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
	int i, j, number, L = 1;
    bool isTag = false;
	
    for (i = 0; i < size; i++)
    {
        if (strlen(codeLine) >= strlen(instructions[i].name) && !strncmp(codeLine, instructions[i].name, strlen(instructions[i].name)))
        {
            for (j = 0; codeLine[j] != ' ' && codeLine[j] != '\t' && codeLine[j] != '\n'; j++); /* Skips instruction */
            codeLine = skipWhiteSpaces(codeLine+j);
            codeLine[strlen(codeLine)-1] = '\0';
            firstOp = strtok(codeLine, ct); /* First operand */
            secondOp = strtok(NULL, ct); /* Second operand */

            /* There never is a third operand, error if there is */
            if (strtok(NULL, ct) != NULL) 
            {
                printf("[%d] Too many operands!\n", lineCount);
                errors = true;
                return -1;
            }

            /* If no operands needed, first operand should be just new line '\n' */
            if (instructions[i].numOfOps == 0)
            {
                if (firstOp != NULL)
                {
                    printf("[%d] Extraneous text after instruction!\n", lineCount);
                    errors = true;
                    return -1;
                }
                    return L;
            }
            
            L++; /* Funct word */

            /* If only one operand needed, error occures if even second operand exists */
            if (instructions[i].numOfOps == 1)
            {
                if (secondOp != NULL)
                {
                    printf("[%d] Too many operands!\n", lineCount);
                    errors = true;
                    return -1;
                }
            }

            /* Identifying first Operand */
            /* Checks for immediate addressing mode */
            if (*firstOp == '#')
            {
                for ((firstOp[j] == '-' ? (j = 2) : (j = 1)); j < strlen(firstOp); j++)
                {
                    if (!isdigit(firstOp[j]))
                    {
                        printf("[%d] A whole number must follow up a # prefix!\n", lineCount);
                        errors = true;
                        return -1;
                    }
                }
                L++;
            }
            else if (*firstOp == 'r') /* Check if first operand is a register or a tag */
            {
                for (j = 1; j < strlen(firstOp); j++)
                {
                    if (!isdigit(firstOp[j]))
                    {
                        isTag = true;
                        break;
                    }
                }
                if (!isTag)
                {
                    number = atoi(firstOp+1);
                    if (number > 15)
                        isTag = true;
                }
            }
            else /* If not a register of immidiate addressing, it's a form of tag addressing */
                isTag = true;
            
            /* Both tag addressing needs 2 words */
            if (isTag)
                L += 2;

            /* If only one operand needed we are done here */
            if (instructions[i].numOfOps == 1)
                return L;
            

            /* Identifying second operand word count */
            isTag = false;
            secondOp = skipWhiteSpaces(secondOp);

            /* Checks for immediate addressing mode */
            if (*secondOp == '#')
            {   
                for ((secondOp[j] == '-') ? (j = 1) : (j = 2); j < strlen(secondOp); j++)
                {
                    if (!isdigit(secondOp[j]))
                    {
                        printf("[%d] A whole number must follow up a # prefix!\n", lineCount);
                        errors = true;
                        return -1;
                    }
                }
                L++;
            }
            else if (*secondOp == 'r')
            {
                for (j = 1; j < strlen(secondOp); j++)
                {
                    if (!isdigit(secondOp[j]))
                    {
                        isTag = true;
                        break;
                    }
                }
                if (!isTag)
                {
                    number = atoi(secondOp+1);
                    if (number > 15)
                    {
                        isTag = true;
                    }
                }
            }
            else /* If not a register of immidiate addressing, it's a form of tag addressing */
            {
                isTag = true;
            }
            
            /* Both tag addressing needs 2 words */
            if (isTag)
                L += 2;
            
            return L;
        }
    }
	printf("[%d] Unknown intsruction!", lineCount);
    errors = true;
    return -1;
}

void printMemory()
{
    int i;
    symPointer = &firstSym;
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
