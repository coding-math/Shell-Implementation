# Shell Implementation

## Project Description

This repository contains a project that implements a Shell in the C programming language. The Shell handles various operators, including `|`, `||`, `&`, `&&`, `<`, `>`, and `>>`. It provides a command-line interface through which users can interact with the system by entering commands.

The Shell program works by accepting user commands as input, parsing them into tokens, and executing the corresponding processes or commands. It also supports input and output redirection, enabling users to redirect command input or output to specific files.

This project was developed by me, [Matheus Silva](http://www.github.com/math-silva), as part of the Operating Systems course at the Federal University of São Paulo (UNIFESP), under the guidance of Professor Dr. Bruno Kimura. It serves as a practical application of the concepts and principles learned in the course.

## How to Run the Project

To run the code, you need to compile the `shell_implementation.c` file and execute the resulting executable. Note that it is necessary to use a Unix-like operating system to run the code

To compile the code, use the following command:
```
$ gcc -o shell_implementation shell_implementation.c
```
After compilation, execute the code with:
```
$ ./shell_implementation <param> <operator> ...
```

## Examples:

```
$ ./shell_implementation ls ">" output.txt
```
This command will execute the `ls` command and redirect the output to a file called `output.txt`.
<br>
```
$ ./shell_implementation command_that_does_not_exist "||" echo "The command failed."
```
Since `command_that_does_not_exist` is not a valid command, it will fail with a non-zero exit status. The `echo` command to the right of the `||` operator will then be executed and will print the message "The command failed." to the terminal.

## Project Objective
The primary goal of this project is to provide a practical implementation of the concepts and principles covered in the Operating Systems course at the Federal University of Sao Paulo (UNIFESP). The project aims to provide students with hands-on experience in developing a basic Shell program in C language that can handle different types of commands and operators.

By working on this project, students can learn how to design and implement a command-line interface that allows users to interact with the system. They can also learn how to parse and interpret user input, manage processes, and handle input and output redirection.

Overall, the project aims to deepen students' understanding of the fundamental concepts of operating systems and enhance their practical skills in developing system-level software.
