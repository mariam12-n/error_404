#include <stdio.h> // for input and output
#include <unistd.h> // for fork, execv, and _exit
#include <sys/wait.h> // for waitpid
#include <string.h> // for strtok and strcmp

#define MAX_INPUT 1024 // maximum input length
#define MAX_ARGS  64 // maximum number of arguments (including command itself)
#define BIN_DIR   "/usr/bin/" // directory to look for commands

int main(void) { // main function

    char input[MAX_INPUT]; // buffer to hold user input
    char *args[MAX_ARGS];// array of pointers to hold command and arguments
    char fullpath[sizeof(BIN_DIR) + MAX_INPUT];// buffer to hold full path of command

    while (1) {// infinite loop to keep the shell running

        printf("new_shell> ");// print prompt
        fflush(stdout);// flush stdout to ensure prompt is printed

        if (fgets(input, MAX_INPUT, stdin) == NULL) {// read user input
            perror("fgets");// if fgets fails, print error and continue
            continue;// if EOF is reached (e.g., user presses Ctrl+D), exit the shell
        }// remove trailing newline character from input

        input[strcspn(input, "\n")] = '\0';// if input is empty, continue to next iteration

        if (input[0] == '\0') {// if input is empty,
            continue;// skip to next iteration to show prompt again
        }

        // split input into args (spaces or tabs)
        int i = 0; // index for args array
        args[i] = strtok(input, " \t"); // get first token (command)
        while (args[i] != NULL && i < MAX_ARGS - 1) {// while there are more tokens and we haven't exceeded max args
            i++;// move to next index
            args[i] = strtok(NULL, " \t");// get next token (argument)
        }
        args[i] = NULL;// null-terminate the args array

        // -------- built-in: exit --------
        if (strcmp(args[0], "exit") == 0) {// if command is "exit"
            printf("\033[0m");// reset color before exiting
            printf("Exiting...\n");// print exit message
            break;// exit the shell
        }

        // -------- helpful tip if user types red/green/reset alone --------
        if (strcmp(args[0], "red") == 0 || strcmp(args[0], "green") == 0 ||  strcmp(args[0], "reset") == 0) {// if user types a color command without "color"

            printf("Tip: use -> color %s\n", args[0]);// print usage tip
            printf("Usage: color red|green|reset\n");// print usage instructions
            continue;// skip to next iteration to show prompt again
        }

        // -------- built-in: color --------
        if (strcmp(args[0], "color") == 0) {// if command is "color"

            if (args[1] == NULL) {// if no color argument is provided
                printf("Usage: color red|green|reset\n");// print usage instructions
            }
            else if (strcmp(args[1], "red") == 0) {// if color argument is "red"
                printf("\033[31m");// set text color to red
            }
            else if (strcmp(args[1], "green") == 0) {// if color argument is "green"
                printf("\033[32m");// set text color to green
            }
            else if (strcmp(args[1], "reset") == 0) {// if color argument is "reset"
                printf("\033[0m");// reset text color to default
            }
            else {
                printf("Unknown color\n");// if color argument is not recognized, print error message
                printf("Usage: color red|green|reset\n");// print usage instructions
            }

            continue;// skip to next iteration to show prompt again
        }

        // run /usr/bin commands
        snprintf(fullpath, sizeof(fullpath), "%s%s", BIN_DIR, args[0]);// construct full path to command

        pid_t pid = fork();// fork a child process to execute the command

        if (pid < 0) {// if fork fails, print error and continue
            perror("fork");// print error message
            continue;// skip to next iteration to show prompt again
        }

        if (pid == 0) {// in child process
            execv(fullpath, args);// execute the command with arguments

            // if execv fails
            printf("Command not found: %s\n", args[0]);// print error message if command is not found
            _exit(127);// exit child process with error code
        }

        waitpid(pid, NULL, 0);// in parent process, wait for child to finish
    }

    return 0;// return success
}