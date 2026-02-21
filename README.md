new_shell (Simple Linux Shell)
Overview
new_shell is a small beginner-friendly shell program written in C.  
It repeatedly shows a prompt, reads a command from the user, and runs it.

This shell supports:
Running commands from /usr/bin/ (example: date, ls)
Built-in command: exit
Built-in command: color red|green|reset
Helpful tip if the user types red, green, or reset without color
Files
new_shell.c

How to Compile
Use GCC to compile: gcc -o new_shell new_shell.c
How to Run
./new_shell
