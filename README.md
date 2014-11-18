Implementing a shell.

Right now it has very limited funcitonality.

The shell will simply parse the command line input and launch whatever you ask it to if the syntax is correct.

Builtins:
--------
The following builtins will work:

exit [value] - 
exit the shell with the value. If value is not given, exit with value 0. 

aecho [-n] [arguments] - 
Echos the command arguments. If the -n flag is given, the newline will not be printed.