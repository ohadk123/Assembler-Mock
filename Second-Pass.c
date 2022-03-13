#include "Second-Pass.h"
#include "First-Pass.h"

int lineCount;
int memLoc;

bool secondPass(Word *memory, int ICF, int DCF, Label firstLabel, Label *labelPointer)
{
    FILE *text;
    char line[MAX_LINE];
    char *startP;
    char *endP;
    char *labelName = (char *)malloc(sizeof(char *));
    int i;
    bool errors = false;
    memLoc = IC_START;

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
            errors = codeEntry(&firstLabel, labelPointer, labelName);
            continue;
        }

        for (i = 0; *startP != ' ' && *startP != '\n' && *startP != '\t'; i++, startP++);
        if (*startP == '\n')
            continue;
        startP = skipWhiteSpaces(startP);
        
        codeLine(memory, startP, labelPointer, &firstLabel);
    }

    printMemory(memory, firstLabel, labelPointer, ICF, DCF);
    remove ("output.txt");
    return errors;
}

bool codeEntry(Label *labelP, Label *labelPointer ,char *name)
{
    while (labelP != labelPointer)
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

void codeLine(Word *memory, char *line, Label *labelPointer, Label *firstLabel)
{

}

void printMemory(Word *memory, Label firstLabel, Label *labelPointer, int IC, int DC)
{
    int i;
    labelPointer = &firstLabel;
    printf("ICF = %d, DCF = %d\n", IC, DC);
    printf("NAME:\tVALUE:\tBASE:\tOFFSET:\tATTRIBUTES:\n");
    while(labelPointer->next != NULL)
    {
        printf("%s\t%d\t%d\t%d\t", labelPointer->name, labelPointer->value, labelPointer->base, labelPointer->offset);
        getAttr(labelPointer->attribute.location, labelPointer->attribute.type);
        labelPointer = labelPointer->next;
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

