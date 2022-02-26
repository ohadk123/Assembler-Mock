#include "Pre-Assembler.h"

int preAssembler(char *fileName)
{
     char **text;   /*Saves the entire pre-assembled code*/
     int t;         /*For loops counter*/
     int size;      /*The amount of lines in the file*/

     /*Error occurred in macro function*/
     if ((size = runMacro(fileName)) == -1)
          return 0;

     /*Allocating memory to text array*/
     text = (char **)malloc(size * (sizeof(char*)));
     for (t = 0; t < size; t++)
          text[t] = (char *)malloc(MAX_LINE * sizeof(char));
     t = 0;
     return 1;
}

int runMacro(char *argv)
{
     char *fileName = (char *)malloc(sizeof(argv)); /*String of the name file in a different location from argv*/
     char line[MAX_LINE];                           /*String of the current line*/
     char *p = line;                                /*A pointer to the string "line"*/
     int i = 0, j, m = 0;                           /*Counters*/
     bool printLine;                                /*Used to skip a line in output process*/

     /*Opens the file, in case of error returns -1*/
     FILE *fp = fopen(strcat(strcpy(fileName, argv), ".as"), "r");
     if (fp == NULL)
     {
          fprintf(stderr, "\nError trying to open %s\n", fileName);
          fprintf(stderr, "The program will continue to the next file\n");
          return -1;
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

          /*Comments need to be ignored*/
          if (!strncmp(p, ";", 1))
               continue;

          /*Start of macro decleration detected, starts macro reading process*/
          if (!strncmp(p, "macro", 5))
          {
               readMacro(p, m, fp);
               m++;
               continue;
          }

          /*Checks if the line is a call for macro*/
          for (j = 0; j < m; j++)
          {
               if (!strncmp(p, macroNames[j], strlen(macroNames[j])))
               {
                    (i = unfoldMacro(i, j));
                    printLine = false;
               }
          }

          /*printLine is false when the line is a macro name*/
          if (printLine)
          {
               fprintf(output, "%s", p);
               i++;
          }
     }
     
     fclose(output);
     return i;
}

void readMacro(char p[], int m, FILE *fp)
{
     char *line = (char *)malloc(MAX_LINE * sizeof(char)); /*Pointer to current line being processed*/

     strcpy(macroNames[m], p + 6); /*Saves the name of the macro*/
     mPtr = macroList[m];          /*Points mPtr to the macro contents array*/

     while (true)
     {
          fgets(line, MAX_LINE, fp);
          line = skipWhiteSpace(line);

          /*endm reached, macro ended*/
          if (!strncmp(line, "endm", 4))
               break;
          else
          {
               strcpy(mPtr, line);   /*Puts the line into the macro definition*/
               mPtr += strlen(mPtr); /*Skips to the end of the string*/
          }
     }
}

int unfoldMacro(int i, int m)
{
     /*char *toPrint = (char *)malloc(MAX_MACRO_SIZE*sizeof(char));*/ /*The string to print as the macro*/
     int j;
     mPtr = macroList[m]; /*Points mPtr to the macro definition*/

     /*counts how many lines is the macro*/
     for (j = 0; *mPtr != '\0'; mPtr++, j++)
     {
          if (*mPtr == '\n')
               i++;
     }
     fprintf(output, "%s", macroList[m]); /*Outputs the macro*/
     return i;
}

char *skipWhiteSpace(char *p)
{
     int i;
     for (i = 0; p[i] != 0 && (p[i] == ' ' || p[i] == '\t'); i++)
          ; /*Count how many white spaces are there*/
     return p + i;
}