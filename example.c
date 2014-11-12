#include <stdio.h>
#include <stdlib.h>
int func_a() {
	return 8;
}

void func_b(int *x) {
	printf("assigning to x at address %p\n", x);
	*x = 10;
}

char **allocate_array() {
	char **argv = malloc(sizeof(char *) * 10);
	printf("assigning to argv at address %p, with the value %p\n", &argv, argv);
	return argv;
}

int allocate_array2(char **argv) {
	argv = malloc(sizeof(char *) * 10);
	printf("assigning to argv at address %p, with the value %p\n", &argv, argv);
	return 1;
}

int allocate_array3(char ***argvp) {
	*argvp = malloc(sizeof(char *) * 10);
	printf("assigning to argv at address %p, with the value %p\n", argvp,
			*argvp);
	return 1;
}

int main() {
	char my_array[10] = "hello bob"; /* can't change the value of my_array */
	char *my_array_pointer = my_array; /* can change the value of my_array_pointer */
	my_array_pointer = "hello mom";

	printf("my_array_pointer address = %p, string = %s\n", my_array_pointer,
			my_array_pointer);

	char **argv = NULL;

	printf(
			"initial: argv is stored at address %p and contains the address %p\n\n",
			&argv, argv);

	argv = NULL;
	argv = allocate_array();
	printf(
			"allocate_array(): argv is stored at address %p and contains the address %p\n\n",
			&argv, argv);

	argv = NULL;
	allocate_array2(argv);
	printf(
			"allocate_array2(): argv is stored at address %p and contains the address %p\n\n",
			&argv, argv);

	argv = NULL;
	allocate_array3(&argv);
	printf(
			"allocate_array3(): argv is stored at address %p and contains the address %p\n\n",
			&argv, argv);

	/**
	 * The thing that this shows is that:
	 * 1. arrays are very different/have a very different declaration than things that allocate space in a
	 * really similar ways - pointers
	 *
	 * 2. passing by reference is really important because it refers to how you're dealing with the memory
	 *
	 */

	char **argv = NULL;

	printf(
			"initial: argv is stored at address %p and contains the address %p\n\n",
			&argv, argv);

	argv = NULL;
	argv = allocate_array();
	printf(
			"allocate_array(): argv is stored at address %p and contains the address %p\n\n",
			&argv, argv);

	argv = NULL;
	allocate_array2(argv);
	printf(
			"allocate_array2(): argv is stored at address %p and contains the address %p\n\n",
			&argv, argv);

	argv = NULL;
	allocate_array3(&argv);
	printf(
			"allocate_array3(): argv is stored at address %p and contains the address %p\n\n",
			&argv, argv);
}
