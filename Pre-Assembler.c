#include "Pre-Assembler.h"

char macroNames[MAX_LINE][MAX_LINE];	  /* Contains the names of the macros of the code */
char macroList[MAX_LINE][MAX_MACRO_SIZE]; /* Contains the contents of the macros of the code */
char *mPtr;						  /* A pointer used for strings in macro process */
FILE *output;                             /* A pointer to the output file which is used for first and second passes (will contain line numbers for error prints) */
FILE *am;					            /* A pointer to the file which the pre-assembler stage code will output to */

bool preAssembler(char *argv)
{
     char *fileName = malloc(sizeof(argv)); /* String of the name file in a different location from argv */
     char line[MAX_LINE];                   /* String of the current line */
     char *linePointer = line;              /* A pointer to the string "line" */
     int i = 1;                             /* Counts the lines in the file */
     int j;                                 /* Loops counter */
     int m = 0;                             /* Counts the macros in the file */
     bool printLine;                        /* Whether a line should be printed to files */

     /* Trying to open the file */
     FILE *fp = fopen(strcat(strcpy(fileName, argv), ".as"), "r");
     if (fp == NULL)
     {
          printf("Error trying to open %s\n", fileName);
          return false;
     }
     fseek(fp, 0, SEEK_SET);

     /* Create the files to write the code into */
     output = fopen("output.txt", "w");
     fseek(output, 0, SEEK_SET);
     am = fopen(strcat(strcpy(fileName, argv), ".am"), "w");
     fseek(output, 0, SEEK_SET);

     while (fgets(line, MAX_LINE, fp) != NULL)
     {
          /* If the line is too long, a '-' char will be add at the start of the line to call the error in first pass */
          if (line[strlen(line)-1] != '\n' && strlen(line) == MAX_LINE-1)
               fprintf(output, "-");
          printLine = true;
          linePointer = line;
          linePointer = skipWhiteSpace(linePointer);

          /* Comments and blank lines are just printed as they are */
          if (!strncmp(linePointer, ";", 1) || !strncmp(linePointer, "\n", 1))
          {
               fprintf(output, "%d\n", i);
               fprintf(am, "%s", line);
               i++;
               continue;
          }

          /* Start of macro decleration detected, starts macro reading process */
          if (!strncmp(linePointer, "macro", 5))
          {
               i += readMacro(linePointer, m, fp);
               m++;
               continue;
          }

          /* Checks if the line is a call for macro */
          for (j = 0; j < m; j++)
          {
               if (!strncmp(linePointer, macroNames[j], strlen(macroNames[j])))
               {
                    unfoldMacro(j);
                    i++;
                    printLine = false;
               }
          }

          if (printLine)
          {
               fprintf(am, "%s", linePointer);
               fprintf(output, "%d %s", i, linePointer);
               i++;
          }
     }
     fprintf(output, "\n");
     free(fileName);
     fclose(fp);
     fclose(output);
     fclose(am);
     return true;
}

int readMacro(char linePointer[], int m, FILE *fp)
{
     char *line;                   /* Pointer to current line being processed*/
     int i = 1;                    /* Macro line Count */
     strcpy(macroNames[m], linePointer + 6); /* Saves the name of the macro */
     mPtr = macroList[m];          /* Points mPtr to the macro contents array */

     /* Iterates for each line in file and saves in the macro list
      * Breaks when reaches "endm" */
     while (true)
     {
          fgets(line, MAX_LINE, fp);
          line = skipWhiteSpace(line);
          i++;

          if (!strncmp(line, "endm", 4))
               break;
          else
          {
               strcpy(mPtr, line);
               mPtr += strlen(mPtr);
          }
     }
     
     return i++;
}

void unfoldMacro(int m)
{
     mPtr = macroList[m];

     fprintf(am, "%s", macroList[m]);
     fprintf(output, "%s", macroList[m]);
     return;
}

char *skipWhiteSpace(char *linePointer)
{
     int i;
     for (i = 0; linePointer[i] != 0 && (linePointer[i] == ' ' || linePointer[i] == '\t'); i++); /* Count how many white spaces are there */
     return linePointer + i;
}
