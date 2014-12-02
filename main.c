/*
 * Questions: when to use perror?
 * ass2.c
 */
#include "proto.h"
/* Shell main */
int argc;
int base;
char** argv;
FILE* flog;

int main (int mainargc, char **mainargv) {

	//collecting arguments, saving globally
	argc = mainargc;
	argv = mainargv;
	base = 0;
	char *prompt = getenv("P1");
	if(!prompt)
			prompt = "% ";

	//streaming input from file or stdin
	char buffer[LINELEN];
	int len;
	FILE *stream = stdin;
	flog = fopen("log_file", "w+"); /* create a log file named log_file.. */

	/* Check how to read input */
	if(argc > 1){
		stream = fopen(argv[1], "r");
		if(!stream){
			perror("Could not find script file");
			exit(NO_FILE);
		}
	}


	//reading input in
	while (1) {

		/* prompt and get line */
		if(stream == stdin)
			fprintf(stderr, "%s", prompt); /* this gets redirected to stderr */

		//puts max LINELEN from stdin into buffer
		if (fgets(buffer, LINELEN, stream) != buffer)
			/* end of file - ctl d - this signifies */
			break;

		/* Get rid of \n at end of buffer. Null byte terminate it instead */
		len = strlen(buffer);
		if (buffer[len - 1] == '\n')
			buffer[len - 1] = 0;

		/* Run it ... */
		processline(buffer);
	}

	if (!feof(stream))
		/* double check on fgets */
		perror("read");

	fclose(flog);
	return 0; /* Also known as exit (0); */
}

/* processes the line - reads and interprets */
void processline(char *line) {

	/* ================== EXPANSION ==================== */
	/* checking for expansions, modifies the input line into the 'expanded version' */
	char newline[LINELEN*2];
	memset(newline, 0, LINELEN*2);
	newline[((LINELEN*2) - 1)] = 0; //null byte terminate it
	int ret = expand(line, newline, LINELEN*2); //error code for expansion...

	/* printing to log and error code */
	fprintf(flog, "Newline: %s, Return value %d\n", newline, ret);
	if(ret != 0){
		printf("Expansion error: %d\n", ret);
		fprintf(flog, "Returned an error code for the expansion of env variables");
		return;
	}

	/* ================= ARG PARSE ==================== */
	/* first process the line - find out whats inside */
	char** line_args = malloc(sizeof(char*));
	int num_args = arg_parse(newline, &line_args);

	int i;
	for(i = 0; i <= num_args; i++){
		fprintf(flog, "Arg number %d: %s\n", i+1, line_args[i]);
	}

	if(num_args < 1) /* if there's nothing there, don't do anything */
		goto end;

	/* ================= BUILT IN ==================== */
	/* then check if its a builtin */
	func_ptr funct;
	int built_in = check_builtin(num_args, line_args, &funct);
	if(built_in){
		int error_code = funct(num_args, line_args); /* todo: you never actually deal with the error codes */
		(error_code == 0 ? : fprintf(flog, "Error code from builtin: %d \n", error_code));
		goto end;
	}

	/* ================= RUN COMMAND ==================== */
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
		char* prog = line_args[0];
		execvp(prog, line_args);
		perror("exec"); //todo: don't need to free params?
		exit(127);
	}

	/* Have the parent wait for child to complete */
	if (wait(&status) < 0)
		perror("wait");

	end:
		free(line_args);//don't need to free in child!
		return;


}

