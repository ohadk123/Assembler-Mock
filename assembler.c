#include "Pre-Assembler.h"

int main(int argc, char *argv[])
{
     int arg;
     char **code;

     if (argc <= 1)
     {
          printf("\nError argc\n");
          exit(1);
     }

     for (arg = 1; arg < argc; arg++)
     {
          if (!(code = preAssembler(argv[arg])))
          {
               printf("Error");
               continue;
          }
          else
          {
               printf("Success");
          }
     }

     return 0;
}