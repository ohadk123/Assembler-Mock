#include "main.h"

char macroNames[MAX_LINE][MAX_LINE];        /* Contains the names of the macros of the code                            */
char macroList[MAX_LINE][MAX_MACRO_SIZE];   /* Contains the contents of the macros of the code                         */
char *mPtr;                                 /* A pointer used for strings in macro process                             */
FILE *output;                               /* A pointer to the file which the pre-assembler stage code will output to */

/**
 * This is function is in charge of managing macros
 * and writing the code into a new file without comments,
 * and unfolded macros
 * @param fileName - The name of the file we want to open
 * @return - the number of lines in the new file or -1 if an error occurred.
 */
int runMacro(char *fileName);

/**
 * This function saves the macros with it's corresponding name
 * @param p - A string containing the name of the macro               
 * @param m - The number of macros already saved in the program       
 * @param fp - A pointer to the file the program reads the code from  
 */
void readMacro(char p[], int m, FILE *fp);

/**
 * This function outputs the macro code instead of macro name
 * @param i -  The amount of lines already printed to the output file
 * @param m - the amount of macros already saved in the programs
 * @return - the number of lines after unfolding the macro
 */
int unfoldMacro(int i, int m);

/**
 * This function skips all white spaces in the start of the line
 * @param p - A pointer of the first char of the line string
 * @return - The pointer of the first char that is not a white space
 */
char *skipWhiteSpace(char *p);

/**
 * This function when recieving a string checks if 
 * the string is identical to the assembly's saved words
 * @param p - A pointer to the string we want to check
 * @return - 1 if it is identical to a saved word, otherwise 0
 */
int nameCheck(char *p);