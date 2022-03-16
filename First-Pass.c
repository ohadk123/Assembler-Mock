#include "First-Pass.h"
#include "Second-Pass.h"

Word memory[MEM_SIZE];
int IC = IC_START, DC = DC_START;
Label firstLabel = {"", 0, 0, 0, {0, 0}, NULL};
Label *labelPointer = &firstLabel;
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

bool firstPass(char *fileName)
{
    FILE *text;
    char line[MAX_LINE];
    char *startP;
    char *endP;
    char *labelName = (char *)malloc(sizeof(char *));
    bool labelFlag = false;
    int i;
    bool errors = false;

    /* Opening the processed code */
    text = fopen("output.txt", "r");
    if (text == NULL)
    {
        printf("Error in pre-Assembler runtime\n");
        printf("The program will continue to the next file\n");
        return false;
    }
    fseek(text, 0, SEEK_SET);

    while (fgets(line, MAX_LINE, text) != NULL)
    {

        labelName = " ";
        startP = line;
        endP = line;
        labelFlag = false;

        if (isdigit(*startP))
        {
            lineCount = atoi(startP);
            while (*startP != ' ' && *startP != '\n')
                startP++;
            if (*startP == ' ')
                startP++;
            endP = startP;
        }

        if (!strcmp(startP, "\n") || strlen(startP) == 0)
            continue;


        while (endP[1] != ' ')
            endP++;

        /* Locating labels */
        if (*endP == ':')
        {
            labelFlag = true;
            labelName = (char *)calloc((endP-startP), sizeof(char));
            for (i = 0; startP[i] != ':'; i++)
                labelName[i] = startP[i];
            
            CHECK_LABEL_NAME

            startP = ++endP;
        }

        startP = skipWhiteSpaces(startP);

        /* Handling data encoding */
        if (!strncmp(startP, DATA, strlen(DATA)) || !strncmp(startP, STRING, strlen(STRING)))
        {
            if (labelFlag)
                labelFlag = assignLabel(labelName, data, none, memLoc);

            if (!strncmp(startP, DATA, strlen(DATA)))
            {
                removeSpaces(startP);
                codeData(startP + strlen(DATA));
            }
            else
            {
                codeString(startP + strlen(STRING) + 2);
            }
            continue;
        }

        /* Entry type handled in second pass */
        if (!strncmp(startP, ENTRY, strlen(ENTRY)))
            continue;

        /* Handling extern type */
        if (!strncmp(startP, EXTERN, strlen(EXTERN)))
        {
            labelName = (char *)malloc(sizeof(endP+2));
            strcpy(labelName, endP+2);
            for (i = 0; i < strlen(labelName); i++)
            {
                if (isspace(labelName[i]))
                    labelName[i] = '\0';
            }
            
            CHECK_LABEL_NAME

            labelFlag = assignLabel(labelName, none, external, 0);
            continue;
        }
        
        if (labelFlag)
            labelFlag = assignLabel(labelName, code, none, memLoc);
        
        L = analizeCode(startP);
        if (L == 0)
            errors = true;
        IC += L;
        memLoc += L;
    }
    
    fclose(text);
    if (!errors) 
        secondPass(memory, IC, DC, firstLabel, labelPointer, fileName);
    return errors;
}

bool assignLabel(char *name, location attrLoc, type attrType, int memo)
{
    labelPointer->name = (char *)malloc(sizeof(name));
    strcpy(labelPointer->name, name);
    labelPointer->value = memo;
    labelPointer->base = (memo/16)*16;
    labelPointer->offset = memo%16;
    labelPointer->attribute.location = attrLoc;
    labelPointer->attribute.type = attrType;
    labelPointer->next = (Label *)malloc(sizeof(Label));
    labelPointer = labelPointer->next;
    labelPointer->name = "";
    return false;
}

bool nameCheck(char *name)
{
    char *names[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    int i;
    for (i = 0; i < sizeof(names)/sizeof(char *); i++)
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

char *skipWhiteSpaces(char *p)
{
     int i;
     for (i = 0; p[i] != 0 && (p[i] == ' ' || p[i] == '\t'); i++); /*Count how many white spaces are there*/
     return p + i;
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
        num = atoi(p);
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
    Label *labelP = &firstLabel;
    while (labelP != labelPointer)
    {
        if (!strcmp(labelP->name, name))
            return false;
        labelP = labelP->next;
    }
    return true;
}

int analizeCode(char *codeLine)
{
    int size = sizeof(instructions)/sizeof(Instruction);
    char *firstOp, *secondOp;
    char ct[2] = ",";
	int i, j, L = 1, l;
    Instruction instruct;
    direction dir;
	
    for (i = 0; i < size; i++)
    {
        instruct = instructions[i];
        if (strlen(codeLine) >= strlen(instruct.name) && !strncmp(codeLine, instruct.name, strlen(instruct.name)))
        {
            for (j = 0; codeLine[j] != ' ' && codeLine[j] != '\t' && codeLine[j] != '\n'; j++); /* Skips instruction */
            codeLine = skipWhiteSpaces(codeLine+j); /* Ignoring white space after instruction */
            codeLine[strlen(codeLine)-1] = '\0'; /* Removing \n from the end for convenience */
            firstOp = strtok(codeLine, ct); /* First operand */
            secondOp = strtok(NULL, ct); /* Second operand */

            memory[memLoc].opcode.A = 1;
            memory[memLoc].number = 1;
            memory[memLoc].number = memory[memLoc].number << instruct.opcode;

            /* There never is a third operand, error if there is */
            if (strtok(NULL, ct) != NULL) 
            {
                printf("[%d] Too many operands!\n", lineCount);
                return 0;
            }

            /* If no operands needed, error if first operand exists
             * Zero operand instructions use only 1 word */
            if (instruct.numOfOps == 0)
            {
                if (firstOp != NULL)
                {
                    printf("[%d] Extraneous text after instruction!\n", lineCount);
                    return 0;
                }
                    return L;
            }
            
            /*Funct word */
            L++;
            memory[memLoc+1].opcode.A = 1;
            memory[memLoc+1].opcode.funct = instruct.funct;

            /* If only one operand needed, error occures if even second operand exists */
            if (instruct.numOfOps == 1 && secondOp != NULL)
            {
                printf("[%d] Too many operands!\n", lineCount);
                return 0;
            }

            dir = instruct.numOfOps == 1 ? destination : origin;

            for (i = 0; i < strlen(firstOp); i++)
            {
                if (isspace(firstOp[i]))
                {
                    j = i;
                    for (; i < strlen(firstOp); i++)
                    {
                        if (!isspace(firstOp[i]))
                        {
                            printf("[%d] Missing comma!\n", lineCount);
                            return 0;
                        }
                    }
                    firstOp[j] = '\0';
                }
            }

            l = identifyAddressingMode(firstOp, instruct, dir ? instruct.destModes : instruct.origModes, dir, L);
            if (l == -1)
                return 0;
            L += l;
            if (instruct.numOfOps == 1) /* If only one operand needed we are done here */
                return L;

            secondOp = skipWhiteSpaces(secondOp);
            l = identifyAddressingMode(secondOp, instruct, instruct.destModes, destination, L);
            if (l == -1)
                return 0;
            L+= l;
            
            return L;
        }
    }
    /* Looped through entire instruction list and not found a correct instruction */
	printf("[%d] Unknown intsruction! %s\n", lineCount, codeLine);
    return 0;
}

int identifyAddressingMode(char *operand, Instruction instruct, bool *modes, direction dir, int L)
{
    #define DIRECTION(dir) dir ? "destination" : "origin"
    int j = 0;
    bool isLabel = false;
    int number;

    /* Checks for immediate addressing mode
     * checks if it's actually a number, skips '-' if it exists
     * Error if there is a char which is not a digit
     * Immidiate addressing mode uses only 1 word */
    if (*operand == '#')
    {   
        for ((operand[j] == '-') ? (j = 1) : (j = 2); j < strlen(operand); j++)
        {
            if (!isdigit(operand[j]) && !isspace(operand[j]))
            {
                printf("[%d] A whole number must follow up a # prefix!\n", lineCount);
                return -1;
            }
            if (isspace(operand[j]))
            {
                for (; j < strlen(operand); j++)
                {
                    if (!isspace(operand[j]))
                    {
                        printf("[%d] Missing comma!\n", lineCount);
                        return -1;
                    }
                }
            }
        }
        if (modes[immediate])
        {
            memory[memLoc+L].opcode.A = 1;
            memory[memLoc+L].number = atoi(operand+1);
            if (dir == destination)
                memory[memLoc+1].opcode.destMode = immediate;
            else
                memory[memLoc+1].opcode.origMode = immediate;
            return 1;
        }
        printf("[%d] instruction doesnt support immidiate addressing mode for %s operand\n", lineCount, DIRECTION(dir));
        return -1;
    }
    /* Check if operand is a register or a label
     * Register name is only a whole number, might be a label if a char is not a digit
     * Error if the char is not a letter as well
     * If all characters are digits, it might be a register or a label 
     * Register names are in range 0-15, else might be a label */
    else if (*operand == 'r')
    {
        for (j = 1; j < strlen(operand); j++)
        {
            if (isspace(operand[j]))
            {
                for (; j < strlen(operand); j++)
                {
                    if (!isspace(operand[j]))
                    {
                        printf("[%d] Missing comma!\n", lineCount);
                        return -1;
                    }
                }
                break;
            }
            if (!isdigit(operand[j]))
            {
                if (!isalpha(operand[j]) && !isspace(operand[j]))
                {
                    printf("[%d] Illegal character in label name!\n", lineCount);
                    return -1;
                }
                isLabel = true;
            }
        }
        if (!isLabel)
        {
            number = atoi(operand+1);
            if (number > 15)
                isLabel = true;
            else
            {
                if (modes[regDirect])
                {
                    if (dir == destination)
                    {
                        memory[memLoc+1].opcode.destMode = regDirect;
                        memory[memLoc+1].opcode.destReg = number;
                    }
                    else
                    {
                        memory[memLoc+1].opcode.origMode = regDirect;
                        memory[memLoc+1].opcode.origReg = number;
                    }
                    return 0;
                }
                printf("[%d] instruction doesnt support direct register addressing mode for %s operand\n", lineCount, DIRECTION(dir));
                return -1;
            }
        }
    }
    
    if (!isalpha(*operand))
    {
        printf("[%d] Illegal starting character in label name!\n", lineCount);
        return -1;
    }

    /* Check index addressing type */
    for (j = 1; j < strlen(operand); j++)
    {
        if (operand[j] == '[' && j+3 < strlen(operand))
        {
            if (operand[j+1] != 'r' || !isdigit(operand[j+2]) || (!isdigit(operand[j+3]) && operand[j+3] != ']'))
            {
                printf("[%d] Unkown register name\n", lineCount);
                return -1;
            }
            if (operand[j+3] != ']' && (j+4 < strlen(operand) && operand[j+4] != ']'))
            {
                printf("[%d] Missing closing square bracket\n", lineCount);
                return -1;
            }

            number = atoi(operand+j+2);

            if (modes[index])
            {
                if (dir)
                {
                    memory[memLoc+1].opcode.destMode = index;
                    memory[memLoc+1].opcode.destReg = number;
                }
                else
                {
                    memory[memLoc+1].opcode.origMode = index;
                    memory[memLoc+1].opcode.origReg = number;
                }
                return 2;
            }

            printf("[%d] Instruction doesn't support index addressing mode for %s operand\n", lineCount, DIRECTION(dir));
            return -1;
        }
    }

    if (modes[direct])
    {
        if (dir)
            memory[memLoc+1].opcode.destMode = direct;
        else
            memory[memLoc+1].opcode.origMode = direct;
        return 2;
    }

    printf("[%d] instruction doesnt support direct addressing modes for %s operand\n", lineCount, DIRECTION(dir));
    return -1;
}