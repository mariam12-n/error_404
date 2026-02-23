#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT 1024
#define MAX_ARGS  64
#define BIN_DIR   "/usr/bin/"

int main() {

    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char fullpath[sizeof(BIN_DIR) + MAX_INPUT];

    while (1) {

        printf("new_shell> ");
       

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            perror("fgets");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (input[0] == '\0') {
            continue;
        }

        // split input into args (spaces or tabs)
        int i = 0;
        args[i] = strtok(input, " \t");
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            i++;
            args[i] = strtok(NULL, " \t");
        }
        args[i] = NULL;

        // -------- built-in: exit --------
        if (strcmp(args[0], "exit") == 0) {
            printf("\033[0m");
            printf("Exiting...\n");
            break;
        }

        // -------- helpful tip if user types red/green/reset alone --------
        if (strcmp(args[0], "red") == 0 ||strcmp(args[0], "green") == 0 ||strcmp(args[0], "reset") == 0) {

            printf("Tip: use -> color %s\n", args[0]);
            printf("Usage: color red|green|reset\n");
            continue;
        }

        // -------- built-in: color --------
        if (strcmp(args[0], "color") == 0) {

            if (args[1] == NULL) {
                printf("Usage: color red|green|reset\n");
            }
            else if (strcmp(args[1], "red") == 0) {
                printf("\033[31m");
            }
            else if (strcmp(args[1], "green") == 0) {
                printf("\033[32m");
            }
            else if (strcmp(args[1], "reset") == 0) {
                printf("\033[0m");
            }
            else {
                printf("Unknown color\n");
                printf("Usage: color red|green|reset\n");
            }

            continue;
        }

        // run /usr/bin commands
        snprintf(fullpath, sizeof(fullpath), "%s%s", BIN_DIR, args[0]);

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            execv(fullpath, args);

            // if execv fails
            printf("Command not found: %s\n", args[0]);
            _exit(127);
        }

        waitpid(pid, NULL, 0);
    }

    return 0;
}
