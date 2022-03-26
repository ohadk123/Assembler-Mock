#include "Second-Pass.h"
#include "First-Pass.h"

int lineCount;
int memLoc;
bool secondErrors = false;
Externals *externalsList;
size_t externListSize = 0;

void secondPass(Word *memory, int ICF, int DCF, Label firstLabel, Label *lastLabel, char *fileName, bool firstErrors)
{
    FILE *text;          /* File containing pre-assembler code */
    char line[MAX_LINE]; /* Hold current line */
    char *startP;        /* Pointer to relavent start of line */
    char *lineP;         /* Used to search inside startP string */
    char *labelName = (char *)malloc(sizeof(char *)); /* Name of label */

    /* Find next free memory word */
    for (memLoc = IC_START; memory[memLoc].opcode.A != 0; memLoc++);
    secondErrors = firstErrors;

    /* Opening the processed code */
    text = fopen("output.txt", "r");
    if (text == NULL)
    {
        printf("Error in pre-Assembler runtime\n");
        printf("The program will continue to the next file\n");
        return;
    }
    fseek(text, 0, SEEK_SET);

    while (fgets(line, MAX_LINE, text) != NULL)
    {
        /* Reset pointers */
        labelName = " ";
        startP = line;
        lineP = line;

        /* Read line number*/
        if (isdigit(*startP))
        {
            lineCount = atoi(startP);
            while (*startP != ' ' && *startP != '\n')
                startP++;
            if (*startP == ' ')
                startP++;
            lineP = startP;
        }

        /* Empty line is ignored */
        if (!strcmp(startP, "\n") || strlen(startP) == 0)
            continue;

        /* Looking for start of instruction */
        while (!isspace(lineP[1]))
            lineP++;
        
        /* Skipping labels */
        if (*lineP == ':')
            startP = lineP + 1;
        startP = skipWhiteSpaces(startP);
        
        /* Skipping data, string and extern declarations */
        if (!strncmp(startP, DATA, strlen(DATA)) || !strncmp(startP, STRING, strlen(STRING)) || !strncmp(startP, EXTERN, strlen(EXTERN)))
            continue;
        
        /* Add entry attribute to label */
        if (!strncmp(startP, ENTRY, strlen(ENTRY)))
        {
            if (*lineP == ':')
                printf("WARNING: [%d] Label is ignored\n", lineCount);
            
            /* Copy label name to string */
            labelName = (char *)malloc(sizeof(startP));
            startP += strlen(ENTRY);
            startP = skipWhiteSpaces(startP);
            strcpy(labelName, startP);

            /* Label name must start with a letter */
            if (!isalpha(*labelName))
            {
                printf("ERROR: [%d] Label name must start with a letter\n", lineCount);
                secondErrors = true;
                continue;
            }
            /* Remove new line char from label name */
            if (labelName[strlen(labelName)-1] == '\n')
                labelName[strlen(labelName)-1] = '\0';
            
            secondErrors = codeEntry(&firstLabel, lastLabel, labelName) || secondErrors;
            continue;
        }
        
        if (!firstErrors)
            codeLine(memory, startP, lastLabel, &firstLabel);
    }

    remove ("output.txt");
    /* Only if no errors occured, print output files */
    if (!secondErrors)
        outputFile(memory, &firstLabel, lastLabel, ICF, DCF, fileName);
    else
        printf("Processing file %s.as failed\n", fileName);

    return;
}

bool codeEntry(Label *labelP, Label *lastLabel ,char *name)
{
    /* Find label in label heap */
    while (labelP != lastLabel && labelP != NULL)
    {
        if (!strcmp(labelP->name, name))
        {
            /* Label can't be both entry and external */
            if (labelP->attribute.type == external)
            {
                printf("ERROR: [%d] External Label \"%s\" can't also be entry\n", lineCount, name);
                return false;
            }
            else
            {
                labelP->attribute.type = entry;
                return false;
            }
        }
        labelP = labelP->next;
    }
    printf("ERROR: [%d] Label %s is not used!\n", lineCount, name);
    return true;
}

void codeLine(Word *memory, char *line, Label *lastLabel, Label *firstLabel)
{
    /* Note: Most error checks are in first pass, no error checks in this function */
    int i, regNum;
    char *ct = ",";
    char *firstOp;
    char *secondOp;

    while (*line && !isspace(*line)) line++; /* Skip to first char */
    line = skipWhiteSpaces(line);

    line[strlen(line)-1] = '\0'; /* Removing \n from the end for convenience */
    firstOp = strtok(line, ct);  /* First operand */
    secondOp = strtok(NULL, ct); /* Second operand */

    /* Zero op instructions are handled fully in first pass */
    if (firstOp == NULL)
    {
        for (; memory[memLoc].quarter.A != 0; memLoc++);
        return;
    }

    /* Remove new line char from operand */
    for (i = 0; i < strlen(firstOp); i++)
    {
        if (isspace(firstOp[i]))
            firstOp[i] = '\0';
    }

    if (*firstOp == '#'); /* Immidiate addressing mode handled fully in first pass */
    else if (*firstOp == 'r') /* rXX can also be a label name, otherwise register handled fully in first pass */
    {
        for (i = 1; i < strlen(firstOp); i++)
            if (isalpha(firstOp[i]))
                secondErrors = !getLabel(memory, lastLabel, firstLabel, firstOp) || secondErrors;
        if (i == strlen(firstOp))
        {
            regNum = atoi(firstOp+1);
            if (regNum < 0 || regNum > 15)
                secondErrors = !getLabel(memory, lastLabel, firstLabel, firstOp) || secondErrors;
        }
    }
    else
    {
        firstOp = strtok(firstOp, "["); /* Label name in index addresing mode is before opening bracket */
        secondErrors = !getLabel(memory, lastLabel, firstLabel, firstOp) || secondErrors;
    }

    /* Only one operand in line */
    if (secondOp == NULL)
    {
        for (; memory[memLoc].quarter.A != 0; memLoc++);
        return;
    }
    secondOp = skipWhiteSpaces(secondOp);

    if (*secondOp == '#'); /* Immidiate addressing mode handled fully in first pass */
    else if (*secondOp == 'r') /* rXX can also be a label name, otherwise register handled fully in first pass */
    {
        for (i = 1; i < strlen(secondOp); i++)
            if (isalpha(secondOp[i]))
                secondErrors = !getLabel(memory, lastLabel, firstLabel, secondOp) || secondErrors;
        if (i == strlen(secondOp))
        {
            regNum = atoi(secondOp+1);
            if (regNum < 0 || regNum > 15)
                secondErrors = !getLabel(memory, lastLabel, firstLabel, secondOp) || secondErrors;
        }
    }
    else
    {
        secondOp = strtok(secondOp, "["); /* Label name in index addresing mode is before opening bracket */
        secondErrors = !getLabel(memory, lastLabel, firstLabel, secondOp) || secondErrors;
    }

    for (; memory[memLoc].quarter.A != 0; memLoc++);
    return;
}

bool getLabel(Word *memory, Label *lastLabel, Label *labelP, char *name)
{
    int i;
    /* Removing white space from end of name */
    for (i = strlen(name)-1; i > 0; i--)
    {
        if (isspace(name[i]))
            name[i] = '\0';
    }

    /* Searching for the label from list of initiallized labels
     * If label is external we add the line address to the externals list */
    for (; labelP != lastLabel && labelP != NULL; labelP = labelP->next)
    {
        if (!strcmp(labelP->name, name))
        {
            switch (labelP->attribute.type)
            {
            case (external) :
                memory[memLoc].opcode.E = 1;
                memory[memLoc+1].opcode.E = 1;
                externalsList = realloc(externalsList, sizeof(Externals) * (externListSize + 1));
                externalsList[externListSize].name = malloc(sizeof(char *));
                strcpy(externalsList[externListSize].name, name);
                externalsList[externListSize].address = memLoc;
                externListSize++;
                break;
            default :
                memory[memLoc].opcode.R = 1;
                memory[memLoc+1].opcode.R = 1;
                break;
            }
            memory[memLoc].number = labelP->base;
            memory[memLoc+1].number = labelP->offset;
            return true;
        }
    }
    
    printf("ERROR: [%d] label name \"%s\" is not recognized\n", lineCount, name);
    return false;
}

void outputFile(Word *memory, Label *firstLabel, Label *lastLabel, int ICF, int DCF, char *fileName)
{
    int i;
    Label *labelP;
    FILE *object;
    FILE *externals;
    FILE *entries;
    int nameLen = strlen(fileName);
    
    /* Create and print memory to .ob file */
    object = fopen(strcat(fileName, ".ob"), "w");
    fseek(object, 0, SEEK_SET);
    fprintf(object, "\t\t\t%d\t%d\n", ICF-IC_START, DCF);
    for (i = 0; i < MEM_SIZE; i++)
    {
        if (memory[i].quarter.A != 0)
        {
            fprintf(object, "%04d\tA%x-B%x-C%x-D%x-E%x\n", i,
                    memory[i].quarter.A, memory[i].quarter.B, memory[i].quarter.C, memory[i].quarter.D, memory[i].quarter.E);
        }
    }
    fclose(object);

    fileName[nameLen] = '\0'; /* Remove extension from file name */

    /* If there is an entry label, create and print to .ent file */
    for (labelP = firstLabel; labelP != lastLabel; labelP = labelP->next)
    {
        if (labelP->attribute.type == entry)
        {
            entries = fopen(strcat(fileName, ".ent"), "w");
            fseek(entries, 0, SEEK_SET);
            break;
        }
    }
    if (entries != NULL)
    {
        for (labelP = firstLabel; labelP != lastLabel; labelP = labelP->next)
        {
            if (labelP->attribute.type == entry)
                fprintf(entries, "%s, %04d, %04d\n", labelP->name, labelP->base, labelP->offset);
        }
        fclose(entries);
    }

    fileName[nameLen] = '\0'; /* Remove extension from file name */

    /* If there is an external label, create and print to .ext file */
    if (externListSize > 0)
    {
        externals = fopen(strcat(fileName, ".ext"), "w");
        fseek(externals, 0, SEEK_SET);
        for (i = 0; i < externListSize; i++)
        {
            fprintf(externals, "%s BASE %04d\n", externalsList[i].name, externalsList[i].address);
            fprintf(externals, "%s OFFSET %04d\n\n", externalsList[i].name, externalsList[i].address+1);
        }
        free(externalsList);
        fclose(externals);
    }

    printf("Processing file %s.as succeeded\n", fileName);
    return;
}