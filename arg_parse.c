/*
 * arg_parse.c
 * FILE #1 - ERROR CODES WILL BE PRECEEDED BY A 1
 *  Created on: Nov 10, 2014
 *      Author: Samiam
 */
# include "proto.h"

/*
 * This function parses the line pointed to by line into an array of strings
 * (referenced via a reference to an array of pointers -argvp)
 */
int arg_parse(char *line, char ***argvp) {
	int len = strlen(line); /* length of the line - for index purposes*/
	int i = 0; /* index */
	int size = 0; /* the number of args */
	int num_quote = 0; /* number of quotations seen thus far */
	int has_smt = 0; /* signifies if there's anything in the string other than whitespace - i.e. process further*/

	/* used as a delimiter for arguments.
	 (1) signifies the previous character was a whitespace, so to increment the 'size' variable */
	int was_white = 1;

	while (i < len) {
		fprintf(flog, "Current char %c\n", line[i]);
		if (line[i] == ' ' && (num_quote % 2) == 0) { //if theres whitespace and you're NOT in the string
			was_white = 1;
			line[i] = '\0'; //put a null byte
		}else if(line[i] == '#' && (num_quote % 2) == 0){ // if there's a comment character and you're not in a string
			//exit this, assume its the end of the line
			line[i] = '\0';
			len = i + 1; //todo: don't know if this is ob1?
		} else if (line[i] == '"') { //there's a quote, so increment the number of quotes
			num_quote++;
			fprintf(flog, "Number of quotes: %d, string: %s\n", num_quote, line + i);
			if(num_quote % 2 == 0) //second quote...put a null byte
				line[i] = '\0';
		}else { //there's a real character
			if (was_white) {
				fprintf(flog, "Hit the next string starting with char: %c, and incrementing the size\n", line[i]);
				size++; //it was white, and now it isn't anymore, its something else so assume its the next arg
				was_white = 0;
				has_smt = 1;
			}
		}
		i++;
		/* todo: pretty sure you need to remove the quotations out of this shit - NEVER DO THAT AGAIN!*/
	}

	/* ----------------- now dealing with global issues ----------- */

	/* first: mismatched quotes */
	if (num_quote % 2 != 0) {
		perror("Mismatched quotes, undefined behavior"); /*todo: mismatched quotes, not a good error message*/
		return INVALID_INPUT;
	}

	/* second straight up has nothing there -
	 * leaving the error handling of inputting just quotes for another method */
	if (has_smt == 0 && num_quote == 0) {
		return SUCCESS;
	}

	fprintf(flog,"=======Size of this string: %d===========\n", size);

	//2. malloc an array of size size
	char** array = (char**) malloc((size + 1) * sizeof(char*)); /*array of pointers */
	array[size] = NULL; /* last index is null */
	i = 0; /* index into the line - position in line */
	int a_index = 0; /* array index - for pointers */
	was_white = 1;
	while (i < len) {
		//		if theres whitespace...
		if (line[i] == '\0') {
			was_white = 1;
		} else { //there is a char there
			//and the previous thing was a null byte
			if (was_white) {
				fprintf(flog, "Adding a ref to the next string starting with char: %c\n", line[i]);
				was_white = 0;
				if(line[i] == '"')
					i++;
				array[a_index] = &line[i];
				a_index++;
			}
		}

		i++;
	}


	*argvp = array;
	return size;
}
