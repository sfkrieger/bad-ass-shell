/*
 * expand.c
 *
 *	 This is the THIRD in the series: therefore all error codes are going to be in the range of -3*
 *  Created on: Nov 17, 2014
 *      Author: Samiam
*
 */
/*error codes */
#define NO_QUOTE -31
#define NEG_VAL -32
#define NO_ROOM -33
#define INTERNAL_ERROR -34
#define NO_USER -35
#include "proto.h"
#include <ctype.h>
#include <sys/types.h>
#include <pwd.h>

/* helper functions for expansion */
int expand_env(char *orig, char *new, int newsize0, places* places);
int repl_pid(char *orig, char *new, int newsize0, places* places);
int repl_arg(char *orig, char *new, int newsize0, places* places);
int repl_arg_n(char *orig, char *new, int newsize0, places* places);
int repl_home(char *orig, char *new, int newsize0, places* places);
int check_space(places* places, int newsize0, int length);
/*
 * This is the public function: Checks orig line, and fills new with the expanded line
 * New is newsize0 in size
 */
int expand(char *orig, char *new, int newsize0) {

	//null byte terminate new
	new[newsize0] = 0;

	/* initialize vars */
	places* places = malloc(sizeof(places));
	places->new_index = 0; /* new array index */
	places->org_index = 0; /* index in original array */
	int succ = SUCCESS; /* the return values of the helper functions - pass it up */
	int not_over = 1; /* flip when finished */

	while (not_over && (places->new_index < newsize0) && (succ == SUCCESS)) {
		char ch = orig[places->org_index++]; //increments index right after..
//		printf("in the while loop...index: %d prev char: %c\n", places->new_index, ch);
		if(ch == 0){
			not_over = 0;
		}else if(ch == '$'){
			char next = orig[places->org_index]; //increments index right after.. (index should always be pointing to the unprocessed thing
			if(isdigit(next)){
				succ = repl_arg(orig, new, newsize0, places);
			}else if(next == '{'){
				places->org_index++;
				succ = expand_env(orig, new, newsize0, places);
			}else if(next == '$'){
				places->org_index++;
				succ = repl_pid(orig, new, newsize0, places);
			}else if(next == '#'){
				places->org_index++;
				succ = repl_arg_n(orig, new, newsize0, places);
			}
		}else if(ch == '~'){
			//if its the first thing passed, or its right after a space, then replace the tilde
			if(places->org_index == 1 || orig[places->org_index - 2] == ' ')
				succ = repl_home(orig, new, newsize0, places);
			else
				new[places->new_index++] = ch;

		}else
			new[places->new_index++] = ch;

		if(succ != 0)
			fprintf(flog, "Error code %d\n", succ);

		fprintf(flog, "Not over val: %d, Eval of remaining length: %d, succ:%d\n", not_over, (places->new_index < newsize0), succ);
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
	if (!end) { /* if there's no closing brace, return a format error */
		return FORMAT_ERROR;
	}


	//put a null byte where the end is so that you can use the string library
	*end = 0;
	char *env_name = getenv(beg);
	int env_size = 0;
	if(env_name){
		char* place_tcpy = (new + places->new_index);
		env_size = strlen(env_name);
		if(!check_space(places, newsize0, env_size)) /* if theres not enough space return error */
				return NO_ROOM;

		strncpy(place_tcpy, env_name, env_size); /* copy the environment name into the new array */

	}

	places->org_index = places->org_index + length + 1; //advance the original index by the size of the path
	fprintf(flog,"The next thing to see on the original string is %s\n", (orig + places->org_index));
	places->new_index = places->new_index + env_size;

	return 0;

}

int repl_pid(char *orig, char *new, int newsize0, places* places){
	int pid = getpid();
	fprintf(flog,"Pid %d\n", pid);
	int num_cpy = snprintf((new + places->new_index), 6, "%d", pid);
	if(!check_space(places, newsize0, num_cpy)){
		/* if theres not enough space return error */
		fprintf(flog, "Not enough room to replace the pid, returning a no room error\n");
		return NO_ROOM;
	}

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
	places->org_index--; //it indexed past to the next one cause it had to fail the condition!
	str[i] = 0;
	n = atoi(str);

	if(n < 0)
		return NEG_VAL;
	else if(n + 1 >= argc){ //beyond the number
		//replace with space
		return SUCCESS;
	}else{
		//if there are arguments passed then argc > 1, and n must be incremented AND shift must be accounted for
		if(argc > 1){
			n++;
			n += base;
		}

		if(n >= argc){
			return SUCCESS;
		}

		char *argv_copy = argv[n];
		int len = strlen(argv[n]);
		char* to_cpy = argv[n];

		if(!check_space(places, newsize0, len)){
				/* if theres not enough space return error */
				fprintf(flog, "Not enough room to replace the arg, returning a no room error\n");
				return NO_ROOM;
		}

		strncpy((new + places->new_index), to_cpy, len);
		places->new_index += len;
	}

	fprintf(flog,"value of n: %d, value of ch: %c, string %s\n", n, ch, str);
	return 0;
}

int repl_arg_n(char *orig, char *new, int newsize0, places* places){
	char buff[4];
	int num_args = argc - base - 1;
	fprintf(flog, "Argc %d, num_args %d\n", argc, num_args);

	int num_digs = snprintf(buff, 4, "%d", num_args); /*if num digs is 4 then the buffer was truncated... */

	if(num_digs > 3){
		fprintf(flog, "Error too many digits in the args, didn't sub\n");
		return INTERNAL_ERROR;
	}

	int space = newsize0 - places->new_index - 1;
	if(num_digs > space)
		return NO_ROOM;

	strncpy((new + places->new_index), buff, num_digs);
	places->new_index = places->new_index + num_digs; //todo: it copies an end of string null byte!! be careful!
	fprintf(flog, "Replacing the #: New string %s\n", new);
	return SUCCESS;
//	(check_spc(new, newsize0, places) < 0) ? return NO_SPACE : sleep(0);


}

int repl_home(char *orig, char *new, int newsize0, places* places){
	char *dir_path;
	int len_dir_path;
	struct passwd *dir_struct;

	int index = places->org_index;
	while((orig[index] != 0) && orig[index] != '/'){
		fprintf(flog, "The value of the char at this index %c\n", orig[index]);
		index++;
	}
	int size_name = index - places->org_index;
		//if name follows then replace with home directory path of the named user
	if(size_name < 2){
		int uid = getuid();
		dir_struct = getpwuid(uid);
	}else{
		char name[size_name + 1];
		name[size_name + 1] = 0;
		strncpy(name, (orig + places->org_index), size_name);
		dir_struct = getpwnam(name);
	}

	if(!dir_struct){
		perror("No user with that name, can't find directory path");
		return NO_USER;
	}
	dir_path = dir_struct->pw_dir;
	len_dir_path = strlen(dir_path); //add one for null byte

	if(len_dir_path >= (newsize0 - places->new_index))
		return NO_ROOM;

	strncpy( (new + places->new_index), dir_path, len_dir_path);
	//update the pointer in orig
	places->org_index += size_name;
	places->new_index += len_dir_path;

	fprintf(flog, "The next thing to see on the original string is %s, and the new string is: %s\n", (orig + places->org_index), new);

	return SUCCESS;

}

/*
 * This function will check if there is enough space to put the data of size length
 * RETURNS: 0 for FAIL! 1 for SUCCESS
 * ASSUMES the length does not include a final null byte, otherwise this is ob1 (but airs on the side of safety)
 */
int check_space(places* places, int newsize0, int length){
	if((places->new_index + length) < newsize0)
		return 1;
	else
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

