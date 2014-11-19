/*
 * expand.c
 *
 *	 This is the THIRD in the series: therefore all error codes are going to be in the range of -3*
 *  Created on: Nov 17, 2014
 *      Author: Samiam
 *
 *      In the function processline(), you will add a call to a new function called ``expand''.
 *
 *     The return int should be a value that means ``expand was successful'' or ``expand had an error
 *     ``expand()'' should not change the original line.
 *
 *     It processes the original line as explained below and produces a new line.
 *
 *     This call to expand must be called before arg_parse() because the new line is what must be passed to arg_parse.
 *
 *     Also, you should just declare an array in processline() for the new line. Do not malloc() space for this new line.
 *
 *     expand() is to process the original line exactly once, starting with the first character (orig[0]) and looping to the last character.
 *     It should do this loop exactly once and do all the required processing during this one loop.
 *     When expand finds text that needs replacing, it replaces the text with the required new text and continues processing the original text at the point after the replaced text.
 *     Do not expand the new text. The basic processing done by expand() is to copy each character, one at a time, to the new string unless it detects that the current character is part of text that needs to be replaced. In that case, expand() adds the new text (expansion text) to the new string as specified by the replacement. Be careful to not write code that is order n squared. Also, remember that strlen(3) is an order n operation.
 In expand(), replace ${NAME} in the original string with the value of the environment variable of the same name. (Yes, the braces are part of the syntax. $NAME is not processed as an environment variable.) This value of the environment variable is placed in the new string. If the name does not exist in the environment, then it is replaced with the null string. This is done for all ${..}s found in the original string. In ALL cases the ${NAME} will not appear in the new string. If you find a ${ and do not find a closing }, print an error message and stop processing the line. Read about the library function getenv(3).
 Note: Replacing one string in the original line with another string in the new line is a task that will be done several more times during this project. It would be best to write helper functions to make it easy in expand() to add similar functionality in a slightly different context.
 */

/*
 * params:
 * 		new should be a pointer to a fixed size array similar to buffer in main().
 *     	newsize is the number of characters in the new array
 *  	signifies maximum numbers of characters that may safely be put into the parameter new.
 */
// FIRST PASS: todo trying it out with passing the whole thing to the helper function when i see a '$' sign
//ALSO: totally assuming the array pointed to by orig is null byte terminated...(which it is..)
#define NO_QUOTE -31
#include "proto.h"

int expand_env(char *orig, char *new, int newsize0, places* places);
int repl_pid(char *orig, char *new, int newsize0, places* places);

int expand(char *orig, char *new, int newsize0) {

	/*
	 * local vars
	 */
	places* places = malloc(sizeof(places));
	places->new_index = 0; /* new array index */
	places->org_index = 0; /* index in original array */
	int succ = 0; /* the return values of the helper functions - pass it up */
	int not_over = 1; /* flip when finished */

	while (not_over && (places->new_index < newsize0)) {
		char ch = orig[places->org_index++]; //increments index right after..
//		printf("Current char %c\n", ch);
		switch (ch) {
		case 0:
			not_over = 0;
			break;
		case '$':{
			char next = orig[places->org_index++]; //increments index right after.. (index should always be pointing to the unprocessed thing
//			printf("Next char %c\n", next);

			switch (next){
				case '{':
					succ = expand_env(orig, new, newsize0, places);
					break;
				case '$':
					succ = repl_pid(orig, new, newsize0, places);
					break;
				default:
					places->org_index--;
//					printf("This should never happen, shouldn't have a $ and nothing else...\n");
					break;
			}

			break;
		}
		default: //just copy the character over..
			new[places->new_index++] = ch;
//			printf("Just copied the char (%c) over in default - new string: %s\n", ch, new);
			break;
		}

		if(succ != 0){
			fprintf(stderr, "Error code %d", succ);
		}


	}
	return succ;
}

int expand_env(char *orig, char *new, int newsize0, places* places) {
	char cur = orig[places->org_index];
	printf("In expand env - current char: %c\n", cur);

	/* begining and end pointers into the original array referencing the env variable to look up*/
	char* beg = (orig + places->org_index);
	char* end = strchr(beg, '}');
	int length = end - beg;
	if (!end) {
		return FORMAT_ERROR;
	}

	//put a null byte where the end is so that you can use the string library?
	*end = 0;
	char *env_name = getenv(beg);
	int env_size = 0;
	if(env_name){
		printf("Found one\n");
		char* dont_seg = (new + places->new_index);
		strcpy(dont_seg, env_name);
		env_size = strlen(env_name);
	}

	//copy the environment name into the new array
	//advance the original index by the size of the path
	places->org_index = places->org_index + length + 1;
//	printf("The next thing to see on the original string is %s\n", (orig + places->org_index));
	places->new_index = places->new_index + env_size;
//	printf(
//			"String stats: should be a null byte: %c, "
//			"passed string: %s, "
//			"this should be the env name: %s, "
//			"this should be the new copied line! %s, "
//			"nothing should be here (its from the new line) %s, "
//			"and the next args %s\n",
//			*end,
//			beg,
//			env_name,
//			new,
//			(new + places->new_index),
//			(orig + places->org_index));
//	printf("\n ----- new line in the expansion : %s ========\n", new);
	return 0;

}

int repl_pid(char *orig, char *new, int newsize0, places* places){
	int pid = getpid();
	printf("Pid %d\n", pid);
	int num_cpy = snprintf((new + places->new_index), 6, "%d", pid);

	places->new_index = places->new_index + num_cpy; //4 bytes for the size of getpid
//	new[places->new_index++] = 0;
	printf("Printing old string after replaced pid %s, printing new string %s\n", (orig + places->org_index), new);
	return SUCCESS;
}

int alt(char *orig, char *new, int newsize0) {
	//loop through once, start at the first char pointed to by orig[0]
	int index = 0;
	int succ = 0;
	int not_over = 1; //todo: find out end termination condition - how you know bufer is over..
	int rep_index = 0; //new array index

	not_over = 0;
	while (not_over && (rep_index < newsize0)) {
		char ch = orig[index];
		switch (ch) {
		case '$':
			//todo: does this increase the size of index? or just pass the added size?
			//			succ = expand_env(orig, new, newsize0, index++, rep_index);
			break;

		}

	}

	return succ;
}
