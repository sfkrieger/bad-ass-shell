# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=gcc
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
#wall means generate more warning
# this file had a -c flag i'm not sure why..
CFLAGS=-Wall -ggdb -g

all: msh

#when tries to make target (LHS) will check mods on RHS files
#recrusively build the shit on RHS
#-o msh : determines the name of the executable
msh: main.o builtin.o arg_parse.o expand.o
	$(CC) -o msh main.o builtin.o arg_parse.o expand.o #links obj files
	
builtin.o: builtin.c
	$(CC) $(CFLAGS) -c builtin.c 

arg_parse.o: arg_parse.c
	$(CC) $(CFLAGS) -c arg_parse.c 

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

expand.o: expand.c
	$(CC) $(CFLAGS) -c expand.c
clean:
	rm -rf *.o *~ lmsh
	
#preprocessor - replaces text in src files (include - will copy standard lib)
#compiler - changes c to ass
#assembler - makes obj files
#linker.. makes exe from obj files