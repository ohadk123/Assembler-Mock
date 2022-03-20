#include "Second-Pass.h"
#include "First-Pass.h"

int lineCount;
int memLoc;
bool secondErrors = false;
Externals *externalsList;
size_t externListSize = 0;

void secondPass(Word *memory, int ICF, int DCF, Label firstLabel, Label *lastLabel, char *fileName)
{
    FILE *text;
    char line[MAX_LINE];
    char *startP;
    char *endP;
    char *labelName = (char *)malloc(sizeof(char *));
    int i;
    for (memLoc = IC_START; memory[memLoc].opcode.A != 0; memLoc++);
    
    externalsList = (Externals *)malloc(0);

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

        labelName = " ";
        startP = line;
        endP = line;

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

        /* Skipping labels */
        if (*endP == ':')
            startP = ++endP;
        startP = skipWhiteSpaces(startP);

        /* Skip data, string and extern */
        if (!strncmp(startP, DATA, strlen(DATA)) || !strncmp(startP, STRING, strlen(STRING)) || !strncmp(startP, EXTERN, strlen(EXTERN)))
            continue;
        
        /* Add entry attribute to label */
        if (!strncmp(startP, ENTRY, strlen(ENTRY)))
        {
            labelName = (char *)malloc(sizeof(endP+2));
            strcpy(labelName, endP+2);
            if (labelName[strlen(labelName)-1] == '\n')
                labelName[strlen(labelName)-1] = '\0';
            secondErrors = codeEntry(&firstLabel, lastLabel, labelName);
            continue;
        }

        for (i = 0; *startP != ' ' && *startP != '\n' && *startP != '\t'; i++, startP++);
        if (*startP == '\n')
            continue;
        startP = skipWhiteSpaces(startP);
        
        codeLine(memory, startP, lastLabel, &firstLabel);
    }
    remove ("output.txt");

    if (!secondErrors)
        outputFile(memory, &firstLabel, lastLabel, ICF, DCF, fileName);
    else
        remove(strcat(fileName, ".am"));

    return;
}

bool codeEntry(Label *labelP, Label *lastLabel ,char *name)
{
    while (labelP != lastLabel)
    {
        if (!strcmp(labelP->name, name))
            {
                labelP->attribute.type = entry;
                return false;
            }
        labelP = labelP->next;
    }
    printf("[%d] Label %s is not used!\n", lineCount, name);
    return true;
}

void codeLine(Word *memory, char *line, Label *lastLabel, Label *firstLabel)
{
    int i;
    char *ct = ",";
    char *firstOp;
    char *secondOp;

    line[strlen(line)-1] = '\0'; /* Removing \n from the end for convenience */
    firstOp = strtok(line, ct); /* First operand */
    secondOp = strtok(NULL, ct); /* Second operand */

    if (firstOp == NULL)
    {
        for (; memory[memLoc].quarter.A != 0; memLoc++);
        return;
    }

    for (i = 0; i < strlen(firstOp); i++)
    {
        if (isspace(firstOp[i]))
            firstOp[i] = '\0';
    }

    if (*firstOp == '#');
    else if (*firstOp == 'r')
    {
        for (i = 1; i < strlen(firstOp); i++)
            if (isalpha(firstOp[i]))
                secondErrors = !getLabel(memory, lastLabel, firstLabel, firstOp);
    }
    else
    {
        firstOp = strtok(firstOp, "[");
        secondErrors = !getLabel(memory, lastLabel, firstLabel, firstOp);
    }

    if (secondOp == NULL)
    {
        for (; memory[memLoc].quarter.A != 0; memLoc++);
        return;
    }
    secondOp = skipWhiteSpaces(secondOp);

    if (*secondOp == '#');
    else if (*secondOp == 'r')
    {
        for (i = 1; i <strlen(secondOp); i++)
            if (isalpha(secondOp[i]))
                secondErrors = !getLabel(memory, lastLabel, firstLabel, secondOp);
    }
    else
    {
        secondOp = strtok(secondOp, "[");
        secondErrors = !getLabel(memory, lastLabel, firstLabel, secondOp);
    }

    for (; memory[memLoc].quarter.A != 0; memLoc++);
    return;
}

bool getLabel(Word *memory, Label *lastLabel, Label *labelP, char *name)
{
    for (; labelP != lastLabel; labelP = labelP->next)
    {
        if (!strcmp(labelP->name, name))
        {
            switch (labelP->attribute.type)
            {
            case (external) :
                memory[memLoc].opcode.E = 1;
                memory[memLoc+1].opcode.E = 1;
                labelP->base = memLoc;
                labelP->offset = memLoc+1;
                externalsList = realloc(externalsList, sizeof(externalsList) + sizeof(Externals));
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
    
    printf("[%d] label name \"%s\" is not recognized\n", lineCount, name);
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

    fileName[nameLen] = '\0';
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
    }
    fclose(entries);

    fileName[nameLen] = '\0';
    if (externListSize > 0)
    {
        externals = fopen(strcat(fileName, ".ext"), "w");
        fseek(externals, 0, SEEK_SET);
        for (i = 0; i < externListSize; i++)
        {
            fprintf(externals, "%s BASE %04d\n", externalsList[i].name, externalsList[i].address);
            fprintf(externals, "%s OFFSET %04d\n\n", externalsList[i].name, externalsList[i].address+1);
        }
    }
    fclose(externals);
    return;
}