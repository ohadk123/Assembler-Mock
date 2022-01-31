#include "preAssembler.h"

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
          
          text = (char **)malloc(size*(sizeof(char *)));
          for (t = 0; t < size; t++)
               *text = (char *)malloc(MAX_LINE*sizeof(char));
          t = 0;

          code = fopen("output.txt", "r");
          if (code == NULL) {printf("\nError occured trying to open file"); continue;}
          fseek(code, 0, SEEK_SET);
          
          while (fgets(text[t], MAX_LINE, code))
               printf("%s", text[t]);
          printf("\ni = %d\n", size);
          remove("output.txt");
          free(text);
     }

     return 0;
}