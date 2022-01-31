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
          
          text = (char **)malloc(size*(sizeof(char *)));
          for (t = 0; t < size; t++)
               *text = (char *)malloc(MAX_LINE*sizeof(char));
          t = 0;

          code = fopen("output.txt", "r");
          if (code == NULL) {printf("\nError occured trying to open file"); continue;}
          fseek(code, 0, SEEK_SET);
          
          while (fgets(text[t], MAX_LINE, code));

          remove("output.txt");
          free(text);
     }

     return 0;
}

int nameCheck(char *p)
{
	int i;
	char *savedWords[] = {"mov", "cmp", "add", "sub", "lea", "bne", "jsr", "red", "prn", "rts", "stop"};
	for (i = 0; savedWords[i]; i++)
	{
          if (!strncmp(p, savedWords[i], strlen(savedWords[i])))
               return true;
	}
	
	if (*p == 'r')
	{
		for (i = 0; i < NUM_OF_REG; i++)
		{
			if (atof(p+1) == i)
				return true;
		}
	}
	
	return false;
}