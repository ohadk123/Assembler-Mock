#include "Second-Pass.h"
#include "First-Pass.h"

int lineCount;
int memLoc;
bool errors = false;

void secondPass(Word *memory, int ICF, int DCF, Label firstLabel, Label *lastLabel, char *fileName)
{
    FILE *text;
    char line[MAX_LINE];
    char *startP;
    char *endP;
    char *labelName = (char *)malloc(sizeof(char *));
    int i;
    bool errors = false;
    for (memLoc = IC_START; memory[memLoc].opcode.A != 0; memLoc++);

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
            errors = codeEntry(&firstLabel, lastLabel, labelName);
            continue;
        }

        for (i = 0; *startP != ' ' && *startP != '\n' && *startP != '\t'; i++, startP++);
        if (*startP == '\n')
            continue;
        startP = skipWhiteSpaces(startP);
        
        codeLine(memory, startP, lastLabel, &firstLabel);
    }

    if (!errors)
        outputFile(memory, &firstLabel, lastLabel, ICF, DCF, fileName);
    else
        remove(strcat(fileName, ".am"));

    remove ("output.txt");
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
                errors = !getLabel(memory, lastLabel, firstLabel, firstOp);
    }
    else
    {
        firstOp = strtok(firstOp, "[");
        errors = !getLabel(memory, lastLabel, firstLabel, firstOp);
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
                errors = !getLabel(memory, lastLabel, firstLabel, secondOp);
    }
    else
    {
        secondOp = strtok(secondOp, "[");
        errors = !getLabel(memory, lastLabel, firstLabel, secondOp);
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
    char *argv = (char *)malloc(sizeof(fileName));
    Label *labelP;
    FILE *object;
    FILE *externals;
    FILE *entries;
    int i;

    /* Generate objects file */
    object = fopen(strcat(strcpy(argv, fileName), ".ob"), "w");
    fseek(object, 0, SEEK_SET);
    fprintf(object, "\t\t\t%d\t%d\n", ICF-IC_START, DCF);
    for (i = 0; i < MEM_SIZE; i++)
    {
        if (memory[i].quarter.A != 0)
            fprintf(object, "%04d\tA%x-B%x-C%x-D%x-E%x\n", i, 
                memory[i].quarter.A, memory[i].quarter.B, memory[i].quarter.C, memory[i].quarter.D, memory[i].quarter.E);
    }
    fclose(object);

    /* Generate externals file */
    for (labelP = firstLabel; labelP != lastLabel; labelP = labelP->next)
    {
        if (labelP->attribute.type == external)
        {
            externals = fopen(strcat(strcpy(argv, fileName), ".ext"), "w");
            fseek(externals, 0, SEEK_SET);
            for (labelP = firstLabel; labelP != lastLabel; labelP = labelP->next)
            {
                if (labelP->attribute.type == external)
                {
                    fprintf(externals, "%s BASE %04d\n", labelP->name, labelP->base);
                    fprintf(externals, "%s OFFSET %04d\n", labelP->name, labelP->offset);
                }
            }
            fclose(externals);
            break;
        }
    }

    /* Generate entries file */
    for (labelP = firstLabel; labelP != lastLabel; labelP = labelP->next)
    {
        if (labelP->attribute.type == entry)
        {
            entries = fopen(strcat(strcpy(argv, fileName), ".ent"), "w");
            fseek(entries, 0, SEEK_SET);
            for (labelP = firstLabel; labelP != lastLabel; labelP = labelP->next)
            {
                if (labelP->attribute.type == entry)
                        fprintf(entries, "%s, %04d, %04d\n", labelP->name, labelP->base, labelP->offset);
            }
            fclose(entries);
            break;
        }
    }

    free(argv);
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

