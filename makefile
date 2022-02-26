Assembler: assembler.o Pre-Assembler.o First-Pass.o
	gcc -g -ansi -Wall -pedantic assembler.o Pre-Assembler.o First-Pass.o -o Assembler

Pre-Assembler.o: Pre-Assembler.c Pre-Assembler.h
	gcc -c -g -ansi -Wall -pedantic Pre-Assembler.c -o Pre-Assembler.o

First-Pass.o: First-Pass.c First-Pass.h
	gcc -c -g -ansi -Wall -pedantic First-Pass.c -o First-Pass.o

assembler.o: Pre-Assembler.c Pre-Assembler.h First-Pass.c First-Pass.h
	gcc -c -g -ansi -Wall -pedantic assembler.c -o assembler.o