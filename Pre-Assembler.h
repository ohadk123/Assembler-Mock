#ifndef PRE_HEADER
#define PRE_HEADER

#include "Global.h"

/**
 * @brief Starts the Pre-Assembler process of unfolding macros
 * Prints the .am file
 * @param argv - The name of the file without file type
 * @return true  - If file opening was successful
 * @return false - If file opening failed
 */
bool preAssembler(char *argv);

/**
 * @brief Saves the file's macros' contents
 * @param p - A string of the macro call
 * @param m - The macro index in macro array (Or amount of macros saved+1)
 * @param fp - A pointer to the input file
 * @return int - Macro line count
 */
int readMacro(char p[], int m, FILE *fp);

/**
 * @brief Unfolds the macro contents when the macro is called
 * 
 * @param m - The index of the macro celled in the macro array
 */
void unfoldMacro(int m);

/**
 * @brief Counts the index of the first non-white space in string
 * @param p - The string to manipulate
 * @return char* - A pointer the first non-white space in string
 */
char *skipWhiteSpace(char *p);

#endif