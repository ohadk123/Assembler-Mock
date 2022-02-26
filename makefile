Assembler: assembler.o Pre-Assembler.o
	gcc -g -ansi -Wall -pedantic assembler.o Pre-Assembler.o -o Assembler
Pre-Assembler.o: Pre-Assembler.c Pre-Assembler.h
	gcc -c -g -ansi -Wall -pedantic Pre-Assembler.c -o Pre-Assembler.o
assembler.o: Pre-Assembler.c Pre-Assembler.h
	gcc -c -g -ansi -Wall -pedantic assembler.c -o assembler.o
