/* scheduler.c: PQSH Scheduler */

#include "pqsh/macros.h"
#include "pqsh/scheduler.h"
#include "pqsh/timestamp.h"
#include <assert.h>
#include <errno.h>
#include <sys/wait.h>

/**
 * Add new command to waiting queue.
 * @param   s	    Pointer to Scheduler structure.
 * @param   fs      File stream to write to.
 * @param   command Command string for new Process.
 **/
void scheduler_add(Scheduler *s, FILE *fs, const char *command) {
    assert(s && fs && command);


    // Create a new process
    Process *p = process_create(command);
    if (!p) {
        error("Failed to create process for command: %s", command);
        return;
    }

    // Assign arrival time
    p->arrival_time = timestamp();


    // Push to waiting queue
    queue_push(&s->waiting, p);

}

/**
 * Display status of queues in Scheduler.
 * @param   s	    Pointer to Scheduler structure.
 * @param   fs      File stream to write to.
 * @param   queue   Bitmask specifying which queues to display.
 **/
void scheduler_status(Scheduler *s, FILE *fs, int queue) {
    // Print metrics
    double avg_turnaround_time = s->finished.size > 0 ? s->total_turnaround_time / s->finished.size : 0;
    double avg_response_time = s->finished.size > 0 ? s->total_response_time / s->finished.size : 0;

    fprintf(fs, "Running = %4lu, Waiting = %4lu, Finished = %4lu, Turnaround = %05.2lf, Response = %05.2lf\n",
            s->running.size, s->waiting.size, s->finished.size,
            avg_turnaround_time, avg_response_time);

    // Helper function to conditionally display queues
    void display_queue(Queue *q, const char *queue_name, int queue_flag) {
        if (queue & queue_flag && q->size > 0) {
            fprintf(fs, "\n%s Queue:\n", queue_name);
            queue_dump(q, fs);
        }
    }

    // Display running, waiting, and finished queues based on the conditions
    display_queue(&s->running, "Running", RUNNING);
    display_queue(&s->waiting, "Waiting", WAITING);
    display_queue(&s->finished, "Finished", FINISHED);
}

/**
 * Schedule next process using appropriate policy.
 * @param   s	    Pointer to Scheduler structure.
 **/
void scheduler_next(Scheduler *s) {
    if (s->policy == FIFO_POLICY) {
        scheduler_fifo(s);
    }
    else if (s->policy == RDRN_POLICY) {
        scheduler_rdrn(s);
    }
    else {
        error("Unknown scheduling policy");
    }
}

/**
 * Wait for any children and remove from queues and update metrics.
 * @param   s	    Pointer to Scheduler structure.
 **/
void scheduler_wait(Scheduler *s) {
    int status;
    pid_t pid;

    // Handle multiple terminated processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        Process *p = queue_remove(&s->running, pid);
        if (!p) {
            info("No process with PID %d found in running queue.", pid);
            continue;
        }

        p->end_time = timestamp();
        queue_push(&s->finished, p);

        // Update Process metrics (e.g., turnaround time)
        double turnaround_time = p->end_time - p->arrival_time;
        double response_time = p->start_time - p->arrival_time;

        // Update Scheduler metrics
        s->total_turnaround_time += turnaround_time;
        s->total_response_time += response_time;

    }

    if (pid < 0 && errno != ECHILD) {
        error("Error waiting for child process");
    }
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
