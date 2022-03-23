#ifndef FIRST_HEADER
#define FIRST_HEADER

#include "Global.h"

#define IC_START 100
#define DC_START 0
#define LABEL_DECLARED 3
#define DATA ".data"
#define STRING ".string"
#define EXTERN ".extern"
#define ENTRY ".entry"

/* Checks that the labe name is valid (label name length and is instruction/register name) */
#define CHECK_LABEL_NAME \
            if (strlen(labelName) > MAX_LABEL) \
            { \
                printf("ERROR: [%d] Label \"%s\" is too long, max label length is %d characters\n", lineCount, labelName, MAX_LABEL); \
                firstErrors = true; \
                continue; \
            } \
            if (!nameCheck(labelName)) \
            { \
                printf("ERROR: [%d] Illegal label name: \"%s\"\n", lineCount, labelName); \
                firstErrors = true; \
                continue; \
            }

/**
 * @brief Handles the first pass of the program
 * Goes line by line and call the relavent methods
 * @return true - Errors occred during first pass
 * @return false - No errors occred during first pass
 */
bool firstPass();

/**
 * @brief Adds a label to the label heap ands its information
 * @param name - The name of the label
 * @param attrLoc - The location attribute of the label
 * @param attrType - The location attribute of the label
 * @param memo - The location in the memory of the label
 * @return false - Always, to reset labelFlag
 */
bool assignLabel(char *name, location attrLoc, type attrType, int memo);

/**
 * @brief Compares label name to illegal label names
 * Such as instruction names and registers names
 * @param name - The name of the label
 * @return true - The label name is invalid
 * @return false - The label name is valid
 */
bool nameCheck(char *name);

/**
 * @brief Counts the index of the first non-white space in string
 * @param p - The string to manipulate
 * @return char* - A pointer the first non-white space in string
 */
char *skipWhiteSpaces(char *p);

/**
 * @brief Writes the binary code to the computer's memory
 * of string types
 * @param p - The string to handle
 */
void codeString(char *p);

/**
 * @brief Writes the binary code to the computer's memory
 * of string types
 * @param p - Contains the data to handle
 */
void codeData(char *p);

/**
 * @brief Analizes a code line
 * Writes to the memory the instruction's opcode and funct words
 * Error checking at line level (amount of operands, commas, etc...)
 * @param codeLine - The line to handle
 * @return int - The word count the code line uses, 0 if error occured
 */
int analizeCode(char *codeLine);

/**
 * @brief  Checks which addressing mode the operand uses
 * Write to the memory if it's available without second pass
 * (Immidiate addressing, Register direct addressing and register inside index addressing)
 * Error checking at operand level (Invalid operand, number size, etc...)
 * @param operand - A string of the operand
 * @param instruct - Which instruction used
 * @param modes - Boolean values for validity of addressing mode for instructions in addressing direction
 * @param dir - The addresing direction (origin operand/destination operand)
 * @param L - The amount of words in the memory heap the instruction required
 * @return int - The amount of words in the memory heap the operand required, -1 if error occcored
 */
int identifyAddressingMode(char *operand, Instruction instruct, bool *modes, direction dir, int L);

#endif
