#include "Pre-Assembler.h"

int main(int argc, char *argv[])
{
     FILE *code;
     char **text;
     int t, size, arg;

     if (argc <= 1) {printf("\nError argc\n"); exit(1);}
     
     for (arg = 1; arg < argc; arg++)
     {
          if ((size = runMacro(argv[arg])) == -1)
               continue;
          
          /*Allocating memory to text array*/
          text = (char **)malloc(size*(sizeof(char *)));
          for (t = 0; t < size; t++)
               *text = (char *)malloc(MAX_LINE*sizeof(char));
          t = 0;

          /*Opens the Pre Assemblered code*/
          code = fopen("output.txt", "r");
          if (code == NULL) {printf("\nError occured trying to open file"); continue;}
          fseek(code, 0, SEEK_SET);
          
          /*Saves the code to array text*/
          while (fgets(text[t], MAX_LINE, code));

          remove("output.txt");
          free(text);
     }

     return 0;
}