/*
char *savedWords[] = {"mov", "cmp", "add", "sub", "lea", "bne", "jsr", "red", "prn", "rts", "stop"}

int nameCheck(char *p)
{
	int i;
	
	for (i = 0; savedWords[i]; i++)
	{
		if (strlen(p) == strlen(savedWords[i])+1)
			if (!strncmp(p, savedWords[i], strlen(savedWords[i])))
				return true;
	}
	
	if (*p == 'r')
	{
		for (i = 0; i < NUM_OF_REG)
		{
			if (atof(p+1) == i)
				return true;
		}
	}
	
	return false;
}
*/
