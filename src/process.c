/* process.c: PQSH Process */

#include "pqsh/macros.h"
#include "pqsh/process.h"
#include "pqsh/timestamp.h"

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

/**
 * Create new process structure given command.
 * @param   command     String with command to execute.
 * @return  Pointer to new process structure
 **/
Process *process_create(const char *command) {

    Process *p = malloc(sizeof(Process));
    if (!p) {
        error("Failed to allocate memory for process: %s", strerror(errno));
        return NULL;
    }

    strncpy(p->command, command, BUFSIZ);
    p->pid = 0;  // assign and then increment
    p->arrival_time = timestamp();
    p->start_time = 0;
    p->end_time = 0;
    p->next = NULL;

    return p;
}
/**
 * Start process by forking and executing the command.
 * @param   p           Pointer to Process structure.
 * @return  Whether or not starting the process was successful
 **/
bool process_start(Process *p) {
    pid_t pid = fork();

    if (pid < 0) {
        error("Failed to fork process: %s", strerror(errno));
        return false;
    } else if (pid == 0) {  // Child process
        char *argv[MAX_ARGUMENTS] = {0};
        int i = 0;

        for (char *token = strtok(p->command, " "); token; token = strtok(NULL, " ")) {
            if (i < MAX_ARGUMENTS - 1) {  // -1 to ensure space for NULL at the end
                argv[i++] = token;
            } else {
                error("Too many arguments in command.");
                exit(EXIT_FAILURE);
            }
        }

        execvp(argv[0], argv);
        error("Failed to execute command: %s", strerror(errno));  // Executing this line means execvp failed
        exit(EXIT_FAILURE);
    } else {  // Parent process
        p->pid = pid;
        p->start_time = timestamp();
        return true;
    }
}

/**
 * Pause process by sending it the appropriate signal.
 * @param   p           Pointer to Process structure.
 * @return  Whether or not sending the signal was successful.
 **/
bool process_pause(Process *p) {
    if (kill(p->pid, SIGSTOP) < 0) {
        error("Failed to pause process %d: %s", p->pid, strerror(errno));
        return false;
    }
    return true;
}

/**
 * Resume process by sending it the appropriate signal.
 * @param   p           Pointer to Process structure.
 * @return  Whether or not sending the signal was successful.
 **/
bool process_resume(Process *p) {
    if (kill(p->pid, SIGCONT) < 0) {
        error("Failed to resume process %d: %s", p->pid, strerror(errno));
        return false;
    }
    return true;
}


/**
 * Delete a given process, freeing the associated memory.
 * @param   p   Pointer to Process structure.
 **/
void process_delete(Process *p) {
    if (p) {
        free(p);
    }
}
/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
