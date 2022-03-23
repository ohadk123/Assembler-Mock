#include "First-Pass.h"
#include "Second-Pass.h"

Word memory[MEM_SIZE];
int IC = IC_START, DC = DC_START;
Label firstLabel = {"", 0, 0, 0, {0, 0}, NULL}; /* First label in label heap */
Label *lastLabelP = &firstLabel;                /* A pointer to the next to last label in label heap */
int memLoc = IC_START;                          /* Memory location index */
int lineCount = 0;                              /* Holds the line number for error output */
bool firstErrors = false;                       /* True if errors are found in first pass */
                                     
                                                /* {immid, direc, index, reg}    {immid, direc, index, reg} */
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
    FILE *text;             /* A pointer to the output file from pre-assembler process */
    Label *labelPointer;    /* Points to a label from the label heap */
    char line[MAX_LINE];    /* The line the programs is processing currently */
    char *startP;           /* Points to the relavent start of the string */
    char *lineP;            /* Used to search inside startP string */
    char *labelName;        /* Holds a label name */
    bool labelFlag = false; /* True if line start with label: call */
    int i;                  /* Loops counter */
    int L;                  /* Function analizeCode return value */
    /* Opening the pre-assembled code */
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
        /* Memory size exceeds virtual computer's memory size */
        if (IC + DC > MEM_SIZE)
        {
            printf("ERROR: Program is too big!\n");
            return true; 
        }

        labelName = " ";
        startP = line;
        lineP = line;
        labelFlag = false;

        /* From Pre-Assembler, if line start with '-' the line is too long */
        if (*startP == '-')
        {
            lineCount = atoi(startP+1);
            printf("ERROR: [%d] Line is too long, max line length is %d\n", lineCount, MAX_LINE);
            firstErrors = true;
            continue;
        }

        /* Reads line number to lineCount */
        if (isdigit(*startP))
        {
            lineCount = atoi(startP);
            while (*startP != ' ' && *startP != '\n')
                startP++;
            if (*startP == ' ')
                startP++;
            lineP = startP;
        }

        /* Nothing to do with empty lines */
        if (!strcmp(startP, "\n") || strlen(startP) == 0)
            continue;

        
        
        /* Locating labels */
        while (!isspace(*lineP) && *lineP != ':')
            lineP++;
        if (*lineP == ':')
        {
            if (!isspace(lineP[1]))
            {
                printf("ERROR: [%d] A space must follow a label name\n", lineCount);
                firstErrors = true;
                continue;
            }
            labelFlag = true;
            labelName = (char *)calloc((lineP-startP), sizeof(char));
            strncpy(labelName, startP, lineP-startP);
            
            CHECK_LABEL_NAME
            /* Can't call the same label twice */
            for (labelPointer = &firstLabel; labelPointer != lastLabelP; labelPointer = labelPointer->next)
            {
                if (!strcmp(labelPointer->name, labelName) && labelPointer->attribute.location == code)
                {
                    printf("ERROR: [%d] Label \"%s\" is already declared\n", lineCount, labelName);
                    firstErrors = LABEL_DECLARED;
                    break;
                }
            }
            
            startP = ++lineP;
        }
        if (firstErrors == LABEL_DECLARED)
        {
            firstErrors = true;
            continue;
        }
        startP = skipWhiteSpaces(startP);
        
        /* Handling data encoding */
        if (!strncmp(startP, DATA, strlen(DATA)) || !strncmp(startP, STRING, strlen(STRING)))
        {
            if (labelFlag)
                labelFlag = assignLabel(labelName, data, none, memLoc);

            if (!strncmp(startP, DATA, strlen(DATA)))
            {
                if (!isspace(startP[strlen(DATA)]))
                {
                    printf("ERROR: [%d] White space must come after .data call\n", lineCount);
                    firstErrors = true;
                    continue;
                }
                codeData(startP + strlen(DATA));
            }
            else
            {
                if (!isspace(startP[strlen(STRING)]))
                {
                    printf("ERROR: [%d] White space must come after .string call\n", lineCount);
                    firstErrors = true;
                    continue;
                }
                codeString(startP + strlen(STRING));
            }
            continue;
        }

        /* Entry type handled in second pass */
        if (!strncmp(startP, ENTRY, strlen(ENTRY)))
        {
            continue;
        }

        /* Handling extern type */
        if (!strncmp(startP, EXTERN, strlen(EXTERN)))
        {
            labelName = (char *)malloc(sizeof(lineP+1));
            lineP = skipWhiteSpaces(lineP);
            strcpy(labelName, lineP);
            if (!isalpha(*labelName))
            {
                printf("ERROR: [%d] Label name must start with a letter\n", lineCount);
                firstErrors = true;
                continue;
            }
            i = 0;
            while (!isspace(labelName[++i]));
            startP = labelName + i;
            startP = skipWhiteSpaces(startP);
            if (*startP != '\n')
            {
                printf("ERROR: [%d] Extraneous text after label name\n", lineCount);
                firstErrors = true;
                continue;
            }
            labelName[i] = '\0';
            
            CHECK_LABEL_NAME

            /* Make sure label declared as external is not already of location code or data */
            for (labelPointer = &firstLabel; labelPointer != lastLabelP; labelPointer = labelPointer->next)
            {
                if (!strcmp(labelPointer->name, labelName))
                {
                    if (labelPointer->attribute.location != none)
                    {
                        printf("ERROR: [%d] External label \"%s\" can't be data or code\n", lineCount, labelName);
                        firstErrors = true;
                        break;
                    }
                    else if (labelPointer->attribute.type == external)
                    {
                        printf("WARNING: [%d] Label \"%s\" is already declared as external\n", lineCount, labelName);
                        break;
                    }
                }
            }
            if (labelPointer == lastLabelP)
                labelFlag = assignLabel(labelName, none, external, 0);
            free(labelName);
            continue;
        }
        
        /* Make sure label of location code is not called as location data as well */
        for (labelPointer = &firstLabel; labelPointer != lastLabelP; labelPointer = labelPointer->next)
        {
            if (!strcmp(labelPointer->name, labelName))
            {
                if (labelPointer->attribute.location == data)
                {
                    printf("ERROR: [%d] code Label \"%s\" can't also be data\n", lineCount, labelName);
                    firstErrors = true;
                    labelFlag = false;
                }
                else
                {
                    labelPointer->attribute.location = code;
                    labelFlag = false;
                }
            }
        }
        if (labelFlag)
            labelFlag = assignLabel(labelName, code, none, memLoc);
        
        /* Line is of type instruction line */
        L = analizeCode(startP);
        if (L == 0)
            firstErrors = true;
        IC += L;
        memLoc += L;
    }
    
    fclose(text);
    secondPass(memory, IC, DC, firstLabel, lastLabelP, fileName, firstErrors);
    return firstErrors;
}

bool assignLabel(char *name, location attrLoc, type attrType, int memo)
{
    lastLabelP->name = (char *)malloc(sizeof(name));
    strcpy(lastLabelP->name, name);
    lastLabelP->value = memo;
    lastLabelP->base = (memo/16)*16;
    lastLabelP->offset = memo%16;
    lastLabelP->attribute.location = attrLoc;
    lastLabelP->attribute.type = attrType;
    lastLabelP->next = (Label *)malloc(sizeof(Label));
    lastLabelP = lastLabelP->next;
    lastLabelP->name = "";
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
    char *quote;
    p = skipWhiteSpaces(p);
    if (*p != '\"')
    {
        printf("ERROR: [%d] String must be inside quotation marks\n", lineCount);
        firstErrors = true;
        return;
    }
    p++;
    quote = strstr(p, "\"");
    if (quote == NULL)
    {
        printf("ERROR: [%d] String must be inside quotation marks\n", lineCount);
        firstErrors = true;
        return;
    }

    while(p != quote)
    {
        memory[memLoc].opcode.A = 1;
        memory[memLoc].number = *p;
        memLoc++;
        DC++;
        p++;
    }
    p++;
    while (*p)
    {
        if (!isspace(*p))
        {
            printf("ERROR: [%d] Extraneous text after closing quotation marks\n", lineCount);
            firstErrors = true;
            return;
        }
        p++;
    }

    memory[memLoc].opcode.A = 1;
    memory[memLoc].number = 0;
    memLoc++;
    DC++;
}

void codeData(char *p)
{
    int num;
    p = skipWhiteSpaces(p);
    if (!strcmp(p, "\n"))
    {
        printf("ERROR: [%d] Expecting numbers after .data\n", lineCount);
        firstErrors = true;
        return;
    }
    while (true)
    {
        p = skipWhiteSpaces(p);
        if (*p == ',')
        {
            printf("ERROR: [%d] Missing number\n", lineCount);
            firstErrors = true;
            return;
        }
        num = atoi(p);
        if (fabs(num) > 32767)
        {
            printf("ERROR: [%d] Numerical value must not exceed 16 bits (+-32767)\n", lineCount);
            firstErrors = true;
            return;
        }
        memory[memLoc].opcode.A = 1;
        memory[memLoc].number = num;
        memLoc++;
        DC++;

        while (isdigit(*++p));
        p = skipWhiteSpaces(p);
        switch (*p)
        {
        case(',') :
            p++;
            break;
        case ('\n') :
            return;
        default :
            if (!isdigit(*p))
                printf("ERROR: [%d] Must have numbers after .data\n", lineCount);
            else
                printf("ERROR: [%d] Missing comma after %d\n", lineCount, num);
            firstErrors = true;
            return;
        }
    }
}

int analizeCode(char *codeLine)
{
    int size = sizeof(instructions)/sizeof(Instruction);
    char *firstOp, *secondOp;
    char ct[2] = ",";
	int i, j, L = 1, l;
    Instruction instruct;
    direction dir;

    for (i = 0; i < strlen(codeLine); i++)
    {
        if (codeLine[i] == ',')
        {
            if (codeLine[i+1] ==  ',')
            {
                printf("ERROR: [%d] Excessive commas\n", lineCount);
                return 0;
            }
            for (i++; i < strlen(codeLine); i++)
            {
                if (!isspace(codeLine[i]))
                    break;
            }
            if (i >= strlen(codeLine)-1)
            {
                printf("ERROR: [%d] Extra comma at end of line\n", lineCount);
                return 0;
            }
        }
    }

    for (i = 0; i < size; i++)
    {
        instruct = instructions[i];
        if (strlen(codeLine) >= strlen(instruct.name) && !strncmp(codeLine, instruct.name, strlen(instruct.name)))
        {
            if (!isspace(codeLine[strlen(instruct.name)]) && codeLine[strlen(instruct.name)] != '\0')
                break;

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
                printf("ERROR: [%d] Too many operands!\n", lineCount);
                return 0;
            }

            /* If no operands needed, error if first operand exists
             * Zero operand instructions use only 1 word */
            if (instruct.numOfOps == 0)
            {
                if (firstOp != NULL)
                {
                    printf("ERROR: [%d] Extraneous text after instruction!\n", lineCount);
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
                printf("ERROR: [%d] Only one operand is needed for instruction \"%s\"\n", lineCount, instruct.name);
                return 0;
            }

            if (instruct.numOfOps == 2 && (secondOp == NULL || firstOp == NULL))
            {
                printf("ERROR: [%d] Missing operands for instruction \"%s\"\n", lineCount, instruct.name);
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
                            printf("ERROR: [%d] Missing comma\n", lineCount);
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
    for (i = 0; i < strlen(codeLine); i++)
    {
        if (isspace(codeLine[i]))
        {
            codeLine[i] = '\0';
            break;
        }
    }
    if (strtok(codeLine, "") == NULL)
    {
        printf("ERROR: [%d] Missing code after label\n", lineCount);
        return 0;
    }
	printf("ERROR: [%d] Unknown intsruction! \"%s\"\n", lineCount, strtok(codeLine, ""));
    return 0;
}

int identifyAddressingMode(char *operand, Instruction instruct, bool *modes, direction dir, int L)
{
    #define DIRECTION(dir) dir ? "destination" : "origin"
    int j = 0;
    bool isLabel = false;
    int number;
    
    if (strlen(operand) > MAX_LABEL)
    {
        printf("ERROR: [%d] Label \"%s\" is too long, max label length is %d characters\n", lineCount, operand, MAX_LABEL);
        return -1;
    }

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
                printf("ERROR: [%d] A whole number must follow up a # prefix!\n", lineCount);
                return -1;
            }
            if (isspace(operand[j]))
            {
                for (; j < strlen(operand); j++)
                {
                    if (!isspace(operand[j]))
                    {
                        printf("ERROR: [%d] Missing comma!\n", lineCount);
                        return -1;
                    }
                }
            }
        }
        if (modes[immediate])
        {
            memory[memLoc+L].opcode.A = 1;
            number = atoi(operand+1);
            if (fabs(number) > 32767)
            {
                printf("ERROR: [%d] Numerical value must not exceed 16 bits (+-32767)!\n", lineCount);
                return -1;
            }
            memory[memLoc+L].number = number;
            if (dir == destination)
                memory[memLoc+1].opcode.destMode = immediate;
            else
                memory[memLoc+1].opcode.origMode = immediate;
            return 1;
        }
        printf("ERROR: [%d] instruction doesnt support immidiate addressing mode for %s operand\n", lineCount, DIRECTION(dir));
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
                        printf("ERROR: [%d] Missing comma!\n", lineCount);
                        return -1;
                    }
                }
                break;
            }
            if (!isdigit(operand[j]))
            {
                if (!isalpha(operand[j]) && !isspace(operand[j]))
                {
                    printf("ERROR: [%d] Illegal character in label name!\n", lineCount);
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
                printf("ERROR: [%d] instruction doesnt support direct register addressing mode for %s operand\n", lineCount, DIRECTION(dir));
                return -1;
            }
        }
    }
    
    if (!isalpha(*operand))
    {
        printf("ERROR: [%d] Illegal starting character in label name!\n", lineCount);
        return -1;
    }

    /* Check index addressing type */
    for (j = 1; j < strlen(operand); j++)
    {
        if (operand[j] == '[' && j+3 < strlen(operand))
        {
            if (operand[j+1] != 'r' || !isdigit(operand[j+2]) || (!isdigit(operand[j+3]) && operand[j+3] != ']'))
            {
                printf("ERROR: [%d] Unkown register name\n", lineCount);
                return -1;
            }
            if (operand[j+3] != ']' && (j+4 < strlen(operand) && operand[j+4] != ']'))
            {
                printf("ERROR: [%d] Missing closing square bracket\n", lineCount);
                return -1;
            }

            number = atoi(operand+j+2);
            if (number < 10 || number > 15)
            {
                printf("ERROR: [%d] Unkown register name\n", lineCount);
                return -1;
            }

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

            printf("ERROR: [%d] Instruction doesn't support index addressing mode for %s operand\n", lineCount, DIRECTION(dir));
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

    printf("ERROR: [%d] instruction doesnt support direct addressing modes for %s operand\n", lineCount, DIRECTION(dir));
    return -1;
}