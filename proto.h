/*
 * ass1.h
 *
 *  Created on: Nov 3, 2014
 *      Author: Samiam
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef ASS1_H_
#define ASS1_H_

/* Constants */
#define LINELEN 1024
#define EXIT 1
#define AECHO 2
#define CHDIR 3
#define ENVSET 4
#define ENVUNSET 5

/* Return codes */
#define SUCCESS 0
#define UNEXPECTED -1
#define INVALID_INPUT -2
#define UNKNOWN_ERROR -3
#define ERROR -1
#define FORMAT_ERROR -4
#define CANT_OPEN 127
#define NO_FILE -5

/* Typedefs */
typedef int (*func_ptr)(int num_args, char **args);
typedef struct {
	int org_index;
	int new_index;
} places;

/* Prototypes */
void processline (char *line);
int arg_parse(char *line, char ***argvp);
int expand (char *orig, char *new, int newsize);

//returns non zero corresponding to the value of the builtin function...
int check_builtin(int num_args, char **argvp, func_ptr *ptr);
int run_builtin(int code, int num_args, char **argvp);

/* globals */
extern int argc;
extern char** argv;
extern FILE* flog;

#endif /* ASS1_H_ */
