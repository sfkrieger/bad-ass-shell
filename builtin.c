/*
 * builtin.c
 *
 *  Created on: Nov 11, 2014
 *      Author: Samiam
 */
#include "proto.h"

int aecho(int num_args, char **argvp) {
	//what if no params are given...
	int new_line = 0;
	int index = 1;
	int last_index = num_args - 1;
	if (num_args == 1) {
		printf("\n");
	} else {
		if (strcmp(argvp[index], "-n") == 0) {
			new_line = 1;
			index++;
		}

		while (index < last_index) {
			printf("%s ", argvp[index]);
			index++;
		}
		if (new_line) {
			//don't print the new line
			printf("%s", argvp[last_index]);
		} else {
			printf("%s\n", argvp[last_index]);
		}
	}
	return SUCCESS;
}

int exit_w(int num_args, char **argvp) {
	//what if no params are given...
	if (num_args > 1) {
		char *param = argvp[1];
		int val = atoi(param);
		if (val) {
			printf("Tried to exit, this is the value of the param %d\n", val);
			exit(val);
		}
	} else {
		printf("Tried to exit with no error code");
		exit(0);
	}
	return 0;
}

/* todo: remove num_args and change argvp */
int check_builtin(int num_args, char **argvp, func_ptr *ptr) {
	char *first = argvp[0];

	if (!strcmp(first, "exit")) {
		printf("Well it works for exit");
		*ptr = &exit_w;
		return EXIT;
	} else if (!strcmp(first, "aecho")) {
		printf("got an aecho");
		*ptr = &aecho;
		return AECHO;
	}
	return 0;
}

int run_builtin(int code, int args, char **argvp) {
	switch (code) {
	case EXIT:
		return exit_w(args, argvp);
	case AECHO:
		return aecho(args, argvp);
	}
	return UNKNOWN_ERROR;
}

