#ifndef SECOND_HEADER
#define SECOND_HEADER

#include "Global.h"

/**
 * @brief Handle the second pass of the assembler
 * Coding of entries and label instructions
 * @param memory - Virtual computer memory array
 * @param ICF - Final IC count from first pass
 * @param DCF - Final DC count from first pass
 * @param firstLabel - First label in label heap
 * @param lastLabel - Pointer to last label in label heap
 * @param fileName - Name of the file
 * @param firstErrors - Boolean if errors occur in first pass
 */
void secondPass(Word *memory, int ICF, int DCF, Label firstLabel, Label *lastLabel, char *fileName, bool firstErrors);

/**
 * @brief Add entry label to label heap
 * @param memory - Virtual computer memory array
 * @param line - Line string 
 * @param lastLabel - Pointer to last label in label heap
 * @param firstLabel - Pointer to the first label in label heap
 */
void codeLine(Word *memory, char *line, Label *lastLabel, Label *firstLabel);

/**
 * @brief Check line operands
 * @param memory - Virtual computer memory array
 * @param line - Line to code operands from
 * @param lastLabel - Pointer to last label in label heap
 * @param firstLabel - Pointer to the first label in label heap
 */
void codeLine(Word *memory, char *line, Label *lastLabel, Label *firstLabel);

/**
 * @brief Code label operand into memory
 * 
 * @param memory - Virtual computer memory array
 * @param lastLabel - Pointer to the last label in label heap
 * @param labelP - Points to a label in label heap
 * @param name - Name of label
 * @return true - If errors occured
 * @return false - If successful
 */
bool getLabel(Word *memory, Label *lastLabel, Label *labelP, char *name);

/**
 * @brief Print the output files of the assembler
 * .ob for virtual computer's memory
 * .ext for external labels list
 * .ent for entry labels list
 * @param memory - Virtual computer memory array
 * @param firstLabel - Pointer to the first label in label heap
 * @param lastLabel - Pointer to last label in label heap
 * @param ICF - Final code count
 * @param DCF - Final object count
 * @param fileName - Name of file
 */
void outputFile(Word *memory, Label *firstLabel, Label *lastLabel, int ICF, int DCF, char *fileName);

#endif