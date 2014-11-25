/*
 * expand.c
 *
 *	 This is the THIRD in the series: therefore all error codes are going to be in the range of -3*
 *  Created on: Nov 17, 2014
 *      Author: Samiam
*
 */
#define NO_QUOTE -31
#define NEG_VAL -32
#include "proto.h"
#include <ctype.h>


int expand_env(char *orig, char *new, int newsize0, places* places);
int repl_pid(char *orig, char *new, int newsize0, places* places);
int repl_arg(char *orig, char *new, int newsize0, places* places);

int expand(char *orig, char *new, int newsize0) {

	/* local vars */
	places* places = malloc(sizeof(places));
	places->new_index = 0; /* new array index */
	places->org_index = 0; /* index in original array */
	int succ = 0; /* the return values of the helper functions - pass it up */
	int not_over = 1; /* flip when finished */

	while (not_over && (places->new_index < newsize0)) {
		char ch = orig[places->org_index++]; //increments index right after..
		if(ch == 0){
			not_over = 0;
		}else if(ch == '$'){
			char next = orig[places->org_index]; //increments index right after.. (index should always be pointing to the unprocessed thing
			if(isdigit(next)){
				succ = repl_arg(orig, new, newsize0, places);
			}else if(next == '{'){
				places->org_index++;
				succ = expand_env(orig, new, newsize0, places);
			}else if('$'){
				places->org_index++;
				succ = repl_pid(orig, new, newsize0, places);
			}
		}else
			new[places->new_index++] = ch;

		if(succ != 0)
			fprintf(flog, "Error code %d\n", succ);
	} //end of string...

	return succ;
}

int expand_env(char *orig, char *new, int newsize0, places* places) {
	char cur = orig[places->org_index];
	fprintf(flog,"In expand env - current char: %c\n", cur);

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
		fprintf(flog,"Found one\n");
		char* dont_seg = (new + places->new_index);
		strcpy(dont_seg, env_name);
		env_size = strlen(env_name);
	}

	//copy the environment name into the new array
	//advance the original index by the size of the path
	places->org_index = places->org_index + length + 1;
	fprintf(flog,"The next thing to see on the original string is %s\n", (orig + places->org_index));
	places->new_index = places->new_index + env_size;

	return 0;

}

int repl_pid(char *orig, char *new, int newsize0, places* places){
	int pid = getpid();
	fprintf(flog,"Pid %d\n", pid);
	int num_cpy = snprintf((new + places->new_index), 6, "%d", pid);

	places->new_index = places->new_index + num_cpy; //4 bytes for the size of getpid
//	new[places->new_index++] = 0;
	fprintf(flog,"Printing old string after replaced pid %s, printing new string %s\n", (orig + places->org_index), new);
	return SUCCESS;
}

int repl_arg(char *orig, char *new, int newsize0, places* places){
	char ch = orig[places->org_index++];
	int n = 0;
	int i = 0;
	char str[argc];

	while(ch > 47 && ch < 58){
		//then its a digit char
		str[i++] = ch;
		ch = orig[places->org_index++];
		fprintf(flog,"char %c\n", ch);
	}

	str[i] = 0;
	n = atoi(str);

	if(n < 0)
		return NEG_VAL;
	else if(n + 1 >= argc){ //beyond the number
		//replace with space
		return SUCCESS;

	}else{
		//if there are arguments passed then argc > 1, and n must be incremented
		if(argc > 1){
			n++;
		}
		char *argv_copy = argv[n];
		int len = strlen(argv[n]);
		char* to_cpy = argv[n];

		strncpy((new + places->new_index), to_cpy, len);
		places->new_index += len;
	}



	fprintf(flog,"value of n: %d, value of ch: %c, string %s\n", n, ch, str);
	return 0;
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
			break;

		}

	}

	return succ;
}

