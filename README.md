# Operating Systems Projects

This repository contains the projects developed by Matheus Silva (RA: 140317) for the Operating Systems course at the Federal University of Sao Paulo (UNIFESP), taught by Professor Dr. Bruno Kimura.

## Project 1: Shell Implementation

### Project Description

This project is a Shell implementation in C language that handles the main operators such as `|`, `||`, `&`, `&&`, `<`, `>`, and `>>`. The Shell program provides a command-line interface that allows the user to interact with the system by entering commands.

The Shell program works by accepting a command as input, parsing it into tokens, and then executing the corresponding process or command. The program also supports input and output redirection, allowing the user to redirect the input or output of a command to a file.

### How to Run the Project

To run the code, you need to compile the `shell_implementation.c` file and execute the resulting executable. Note that it is necessary to use a Unix-like operating system to run the code

To compile the code, use the following command:
```
$ gcc -o shell_implementation shell_implementation.c
```
After compilation, execute the code with:
```
$ ./shell_implementation <param> <operator> ...
```

### Examples:

```
$ ./shell_implementation ls ">" output.txt
```
This command will execute the `ls` command and redirect the output to a file called `output.txt`.
<br>
```
$ ./shell_implementation command_that_does_not_exist "||" echo "The command failed."
```
Since `command_that_does_not_exist` is not a valid command, it will fail with a non-zero exit status. The `echo` command to the right of the `||` operator will then be executed and will print the message "The command failed." to the terminal.
### Project Objective
The primary goal of this project is to provide a practical implementation of the concepts and principles covered in the Operating Systems course at the Federal University of Sao Paulo (UNIFESP). The project aims to provide students with hands-on experience in developing a basic Shell program in C language that can handle different types of commands and operators.

By working on this project, students can learn how to design and implement a command-line interface that allows users to interact with the system. They can also learn how to parse and interpret user input, manage processes, and handle input and output redirection.

Overall, the project aims to deepen students' understanding of the fundamental concepts of operating systems and enhance their practical skills in developing system-level software.

## Project 2: ... 