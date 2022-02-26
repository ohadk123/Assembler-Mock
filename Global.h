#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 81					/*Maximum allowed length of line*/
#define MAX_MACRO_SIZE 6 * MAX_LINE /*Maximum length allowed of macro*/

typedef enum boolean
{
	false = 0,
	true = 1
} bool;
