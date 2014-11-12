/*
 * ass1.h
 *
 *  Created on: Nov 3, 2014
 *      Author: Samiam
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef ASS1_H_
#define ASS1_H_



/* Constants */
#define LINELEN 1024
#define EXIT 1
#define AECHO 2

/* Return codes */
#define SUCCESS 0
#define UNEXPECTED -1
#define INVALID_INPUT -2
#define UNKNOWN_ERROR -3
#define ERROR -1

/* Typedefs */
typedef int (*func_ptr)(int num_args, char **args);

/* Prototypes */
void processline (char *line);
int arg_parse(char *line, char ***argvp);

//returns 0 if this isn't a builtin function
//returns non zero corresponding to the value of the builtin function...
int check_builtin(int num_args, char **argvp, func_ptr *ptr);
int run_builtin(int code, int num_args, char **argvp);


#endif /* ASS1_H_ */
