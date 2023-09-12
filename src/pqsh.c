#include "pqsh/macros.h"
#include "pqsh/options.h"
#include "pqsh/scheduler.h"
#include "pqsh/signal.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

/* Global Variables */

Scheduler PQShellScheduler = {
    .policy    = FIFO_POLICY,
    .cores     = 1,
    .timeout   = 250000,
};

/* Help Message */

void help() {
    printf("Commands:\n");
    printf("  add    command    Add command to waiting queue.\n");
    printf("  status [queue]    Display status of specified queue (default is all).\n");
    printf("  help              Display help message.\n");
    printf("  exit|quit         Exit shell.\n");
}

/* Main Execution */

int main(int argc, char *argv[]) {
    Scheduler *s = &PQShellScheduler;

    /* Parse command line options */
    if (!parse_command_line_options(argc, argv, s)) {
        error("Failed to parse command line options");
        return EXIT_FAILURE;
    }

    /* Register signal handlers */
    if (!signal_register(SIGALRM, 0, sigalrm_handler)) {
        error("Failed to register SIGALRM handler");
        return EXIT_FAILURE;
    }

    /* Start timer interrupt */
    struct itimerval it = {
        .it_interval = { 0, s->timeout },
        .it_value = { 0, s->timeout }
    };
    if (setitimer(ITIMER_REAL, &it, NULL) < 0) {
        error("Failed to set timer");
        return EXIT_FAILURE;
    }

    /* Process shell commands */
    char input_command[BUFSIZ] = "";
    char command_name[BUFSIZ] = "";
    char argument[BUFSIZ] = "";

    while (1) { // Changed loop condition to always true for clarity
        printf("\nPQSH> ");
        
        memset(input_command, 0, BUFSIZ); // Clear the buffer

        if (!fgets(input_command, BUFSIZ, stdin)) {
            if (feof(stdin)) {
                break; // Exit if EOF is encountered
            }
            // Check if fgets was interrupted by a signal
            if (errno == EINTR) {
                continue;
            }
        }
            chomp(input_command);
        char *token = strtok(input_command, " \t\n");

        if (token) {
            strncpy(command_name, token, BUFSIZ - 1);
            command_name[BUFSIZ - 1] = '\0'; // Ensure null-termination

            token = strtok(NULL, "\n");
            if (token) {
                strncpy(argument, token, BUFSIZ - 1);
                argument[BUFSIZ - 1] = '\0'; // Ensure null-termination
            } else {
                argument[0] = '\0';
            }
        }

        if (streq(command_name, "help")) {
            help();
        } else if (streq(command_name, "add")) {
            scheduler_add(s, stdout, argument);
        printf("Added process \"%s\" to waiting queue.\n", argument);
        } else if (streq(command_name, "status")) {
            int queue_flag;
            if (streq(argument, "running")) {
                queue_flag = RUNNING;
            } else if (streq(argument, "waiting")) {
                queue_flag = WAITING;
            } else if (streq(argument, "finished")) {
                queue_flag = FINISHED;
            } else {
                queue_flag = RUNNING | WAITING | FINISHED;  // Default to show all if no specific queue is mentioned.
            }
            scheduler_status(s, stdout, queue_flag);
        } else if (streq(command_name, "exit") || streq(command_name, "quit")) {
            break;
        } else if (strlen(command_name)) {
            printf("Unknown command: %s\n", command_name);
        }
    }

    return EXIT_SUCCESS;
}
