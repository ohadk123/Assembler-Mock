#include "Pre-Assembler.h"

char macroNames[MAX_LINE][MAX_LINE];	  /* Contains the names of the macros of the code                            */
char macroList[MAX_LINE][MAX_MACRO_SIZE]; /* Contains the contents of the macros of the code                         */
char *mPtr;						  /* A pointer used for strings in macro process                             */
FILE *output;					       /* A pointer to the file which the pre-assembler stage code will output to */

bool preAssembler(char *argv)
{
     char *fileName = (char *)malloc(sizeof(argv)); /* String of the name file in a different location from argv */
     char line[MAX_LINE];                           /* String of the current line                                */
     char *p = line;                                /* A pointer to the string "line"                            */
     int i = 1, j, m = 0;                           /* Counters                                                  */
     bool printLine;                                /* Used to skip a line in output process                     */

     /*Opens the file, in case of error returns -1*/
     FILE *fp = fopen(strcat(strcpy(fileName, argv), ".as"), "r");
     if (fp == NULL)
     {
          printf("Error trying to open %s\n", fileName);
          printf("The program will continue to the next file\n");
          return false;
     }
     fseek(fp, 0, SEEK_SET);

     /*Creates a new file to write the code into*/
     output = fopen("output.txt", "w");
     fseek(output, 0, SEEK_SET);

     /*Process the code line by line*/
     while (fgets(line, MAX_LINE, fp) != NULL)
     {
          printLine = true;
          p = line;
          p = skipWhiteSpace(p);

          /*Comments and blank lines should be ignored*/
          if (!strncmp(p, ";", 1) || !strncmp(p, "\n", 1))
          {
               fprintf(output, "%d\n", i);
               i++;
               continue;
          }

          /*Start of macro decleration detected, starts macro reading process*/
          if (!strncmp(p, "macro", 5))
          {
               i += readMacro(p, m, fp);
               m++;
               continue;
          }

          /*Checks if the line is a call for macro*/
          for (j = 0; j < m; j++)
          {
               if (!strncmp(p, macroNames[j], strlen(macroNames[j])))
               {
                    unfoldMacro(j);
                    i++;
                    printLine = false;
               }
          }

          /*printLine is false when the line is a macro name*/
          if (printLine)
          {
               fprintf(output, "%d %s", i, p);
               i++;
          }
     }
     
     free(fileName);
     fclose(fp);
     fclose(output);
     return true;
}

int readMacro(char p[], int m, FILE *fp)
{
     char *line; /* Pointer to current line being processed*/
     int i = 1;
     strcpy(macroNames[m], p + 6); /*Saves the name of the macro*/
     mPtr = macroList[m];          /*Points mPtr to the macro contents array*/

     while (true)
     {
          fgets(line, MAX_LINE, fp);
          line = skipWhiteSpace(line);
          i++;

          /*endm reached, macro ended*/
          if (!strncmp(line, "endm", 4))
               break;
          else
          {
               strcpy(mPtr, line);   /*Puts the line into the macro definition*/
               mPtr += strlen(mPtr); /*Skips to the end of the string*/
          }
     }
     
     return i++;
}

void unfoldMacro(int m)
{
     mPtr = macroList[m]; /*Points mPtr to the macro definition*/

     fprintf(output, "%s", macroList[m]); /*Outputs the macro*/
     return;
}

char *skipWhiteSpace(char *p)
{
     int i;
     for (i = 0; p[i] != 0 && (p[i] == ' ' || p[i] == '\t'); i++); /* Count how many white spaces are there */
     return p + i;
}
