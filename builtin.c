/*
 * builtin.c
 *
 *  Created on: Nov 11, 2014
 *      Author: Samiam
 */
#include "proto.h"

//helpers
int aecho(int num_args, char **argvp);
int ch_dir(int num_args, char **argvp);
int envset(int num_args, char **argvp);
int envunset(int num_args, char **argvp);
int exit_w(int num_args, char **argvp);

#define TOO_FEW_ARGS -21
#define ENV_UNSET -22
#define ENV_SET -23
#define NO_HOME -24
#define NO_DIR_CHANGE -25

/* todo: remove num_args and change argvp */
int check_builtin(int num_args, char **argvp, func_ptr *ptr) {
	char *first = argvp[0];

	if (!strcmp(first, "exit")) {
		*ptr = &exit_w;
		return EXIT;
	} else if (!strcmp(first, "aecho")) {
		fprintf(flog,"got an aecho\n");
		*ptr = &aecho;
		return AECHO;
	} else if (!strcmp(first, "envset")) {
		fprintf(flog,"In envset\n");
		*ptr = &envset;
		return ENVSET;
	} else if(!strcmp(first, "envunset")) {
		*ptr = &envunset;
		fprintf(flog,"In envunset\n");
		return ENVUNSET;
	} else if(!strcmp(first, "cd")){
		*ptr = &ch_dir;
		return CHDIR;
	}
	return 0;
}

/*
 * WARNING - UNUSED!
 */
int run_builtin(int code, int args, char **argvp) {
	switch (code) {
	case EXIT:
		return exit_w(args, argvp);
	case AECHO:
		return aecho(args, argvp);
	case ENVSET:
		return envset(args, argvp);
	case ENVUNSET:
		return envunset(args, argvp);
	}
	return UNKNOWN_ERROR;
}

int aecho(int num_args, char **argvp) {
	//what if no params are given...
	int new_line = 0;
	int index = 1;
	int last_index = num_args - 1;
	if (num_args == 1) {
		fprintf(flog,"\n");
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

int ch_dir(int num_args, char **argvp){
	char* dir;
	if(num_args < 2){
		dir = getenv("HOME");
		if(!dir)
			return NO_HOME;

	}else
		dir = argvp[1];

	int err = chdir(dir);

	char buffer[LINELEN];
	if (getcwd (buffer, LINELEN) == buffer)
	        fprintf(flog,"Your cur working dir %s, error code %d, argument %s\n", buffer, err, dir);

	if(!err)
		return NO_DIR_CHANGE;

	return SUCCESS;
}

int envset(int num_args, char **argvp){
	if(num_args < 3){
		return TOO_FEW_ARGS;
	}

	char* name = argvp[1];
	char* value = argvp[2];
	int success = setenv(name, value, 1);

	if(success != 0){
		fprintf(flog,"Couldn't set the env variable for some reason: %d\n", success);
		return ENV_SET;
	}

	return SUCCESS;
}

int envunset(int num_args, char **argvp){

	if(num_args < 2){
		return TOO_FEW_ARGS;
	}

	char* name = argvp[1];
	fprintf(flog,"Trying to unset the env var %s", name);

	int success = unsetenv(name);
	if(success != 0){
		fprintf(flog,"Couldn't unset the env variable for some reason: %d\n", success);
		return ENV_UNSET;
	}

	return SUCCESS;
}

int exit_w(int num_args, char **argvp) {
	//what if no params are given...
	if (num_args > 1) {
		char *param = argvp[1];
		int val = atoi(param);
		if (val) {
			fprintf(flog,"Tried to exit, this is the value of the param %d\n", val);
			exit(val);
		}
	} else {
		fprintf(flog,"SEEE YA LATER BETCCH\n");
		exit(0);
	}
	return 0;
}


