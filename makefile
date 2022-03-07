Assembler: assembler.o Pre-Assembler.o First-Pass.o
	gcc -g -ansi -Wall -pedantic assembler.o Pre-Assembler.o First-Pass.o -lm -o Assembler

Pre-Assembler.o: Pre-Assembler.c Pre-Assembler.h
	gcc -c -g -ansi -Wall -pedantic Pre-Assembler.c -lm -o Pre-Assembler.o

First-Pass.o: First-Pass.c First-Pass.h
	gcc -c -g -ansi -Wall -pedantic First-Pass.c -lm -o First-Pass.o

assembler.o: Pre-Assembler.c Pre-Assembler.h First-Pass.c First-Pass.h
	gcc -c -g -ansi -Wall -pedantic assembler.c -lm -o assembler.o