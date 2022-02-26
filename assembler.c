#include "Pre-Assembler.h"

int main(int argc, char *argv[])
{
     int arg;
     if (argc <= 1)
     {
          printf("\nError argc\n");
          exit(1);
     }

     for (arg = 1; arg < argc; arg++)
     {
          if (!preAssembler(argv[arg]))
          {
               continue;
          }
          else
          {
          }
     }

     return 0;
}