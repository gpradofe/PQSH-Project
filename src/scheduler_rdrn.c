/* scheduler_rdrn.c: PQSH Round Robin Scheduler */

#include "pqsh/macros.h"
#include "pqsh/scheduler.h"
#include <errno.h>
#include <assert.h>
#include <signal.h>
/**
 * Schedule next process using round robin policy:
 *
 *  1. If there no waiting processes, then do nothing.
 *
 *  2. Move one process from front of running queue and place in back of
 *  waiting queue.
 *
 *  3. Move one process from front of waiting queue and place in back of
 *  running queue.
 *
 * @param   s	    Scheduler structure
 **/
void scheduler_rdrn(Scheduler *s) {

    // Move processes from running to waiting until we have free cores
    if(s->running.size == s->cores) {
        Process* waiting_front = queue_pop(&s->running);
        process_pause(waiting_front);
        queue_push(&s->waiting, waiting_front);
    }

    // Fill the available cores with waiting processes
    while (s->running.size < s->cores && s->waiting.size != 0) {
        Process* waiting_front = queue_pop(&s->waiting);

        if (waiting_front->pid == 0) {
            process_start(waiting_front);
        } else {
            process_resume(waiting_front);
        }

        queue_push(&s->running, waiting_front);
    }
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
