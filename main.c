/*
 * ass2.c
 */
#include "proto.h"
/* Shell main */

int main(void) {
	char buffer[LINELEN];
	int len;

	while (1) {

		/* prompt and get line */
		fprintf(stderr, "%% "); /* this gets redirected to stderr */

		//puts max LINELEN from stdin into buffer
		if (fgets(buffer, LINELEN, stdin) != buffer)
			/* end of file - ctl d - this signifies */
			break;

		/* Get rid of \n at end of buffer. */
		len = strlen(buffer);
		if (buffer[len - 1] == '\n')
			buffer[len - 1] = 0;

		/* Run it ... */
		processline(buffer);
	}

	if (!feof(stdin))
		/* double check on fgets */
		perror("read");

	return 0; /* Also known as exit (0); */
}

/* processes the line - reads and interprets */
void processline(char *line) {
	char newline[LINELEN*2];
	memset(newline, 0, LINELEN*2);
	newline[((LINELEN*2) - 1)] = 0;
	int ret = expand(line, newline, LINELEN*2);
	printf("Newline: %s, Return value %d\n", newline, ret);

	/* first process the line - find out whats inside */
	char** argv = malloc(sizeof(char***));
	int num_args = arg_parse(line, &argv);

	if(num_args == 0){ /* if there's nothing there, don't do anything */
		goto end;
	}

	/* then check if its a builtin */
	func_ptr funct;
	int built_in = check_builtin(num_args, argv, &funct);
//	int built_in = check_builtin(num_args, argv);
	if(built_in){
//		int error_code = run_builtin(built_in, num_args, argv); /* todo: you never actually deal with the error codes */
		int error_code = funct(num_args, argv); /* todo: you never actually deal with the error codes */

		printf("Error code from builtin: %d \n", error_code);
		goto end;
	}

	/* Start a new process to do the job. */
	pid_t cpid;
	int status;
	cpid = fork();
	if (cpid < 0) {
		perror("fork");
		goto end;
	}

	/* Check for who we are! value of cpid is the only thing different, both processes are doing the same thing now! */
	if (cpid == 0) {
		/* We are the child! */
		char* prog = argv[0];
		execvp(prog, argv);
		perror("exec"); //todo: don't need to free params?
		exit(127);
	}

	/* Have the parent wait for child to complete */
	if (wait(&status) < 0)
		perror("wait");

	end:
		free(argv);//don't need to free in child!
		return;


}

