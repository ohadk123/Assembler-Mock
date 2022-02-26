#include "Global.h"

typedef union Word
{
     struct
     {
          unsigned int E :4;
          unsigned int D :4;
          unsigned int C :4;
          unsigned int B :4;
          unsigned int A :4;
     } quarter;

     struct
     {
          unsigned int destType :2;
          unsigned int destReg :4;
          unsigned int origType :2;
          unsigned int origReg :4;
          unsigned int funct :4;
          unsigned int E :1;
          unsigned int R :1;
          unsigned int A :1;
          unsigned int o :1;
     } opcode;

     unsigned short base;
     unsigned short offset;
     unsigned short number;
} Word;