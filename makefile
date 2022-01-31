Assembler: assembler.o preAssembler.o
	gcc -g -ansi -Wall -pedantic assembler.o preAssembler.o -o Assembler
preAssembler.o: preAssembler.c preAssembler.h main.h
	gcc -c -g -ansi -Wall -pedantic preAssembler.c -o preAssembler.o
assembler.o: preAssembler.c preAssembler.h
	gcc -c -g -ansi -Wall -pedantic assembler.c -o assembler.o
