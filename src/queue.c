/* queue.c: PQSH Queue */

#include "pqsh/macros.h"
#include "pqsh/queue.h"

#include <assert.h>

/**
 * Push process to back of queue.
 * @param q     Pointer to Queue structure.
 **/
void queue_push(Queue *q, Process *p) {
    assert(q && p);

    if (!q->head) {  // if queue is empty
        q->head = q->tail = p;
    } else {
        q->tail->next = p;
        q->tail = p;
    }
    p->next = NULL;
    q->size++;
}

/**
 * Pop process from front of queue.
 * @param q     Pointer to Queue structure.
 * @return  Process from front of queue.
 **/
Process *queue_pop(Queue *q) {
    if (!q->head) return NULL;

    Process *p = q->head;
    q->head = q->head->next;
    if (!q->head) q->tail = NULL;
    q->size--;

    p->next = NULL;  // Disconnect the process from the queue.
    return p;
}
/**
 * Remove and return process with specified pid.
 * @param q     Pointer to Queue structure.
 * @param pid   Pid of process to return.
 * @return  Process from Queue with specified pid.
 **/
Process *queue_remove(Queue *q, pid_t pid) {
    Process *current = q->head;
    Process *previous = NULL;

    while (current) {
        if (current->pid == pid) {
            if (previous) {
                previous->next = current->next;
            } else {
                q->head = current->next;
            }
            
            if (q->tail == current) {
                q->tail = previous;
            }

            q->size--;
            current->next = NULL;  // Disconnect the process from the queue.
            return current;
        }
        previous = current;
        current = current->next;
    }
    
    return NULL;
}

/**
 * Dump the contents of the Queue to the specified stream.
 * @param q     Queue structure.
 * @param fs    Output file stream.
 **/
void queue_dump(Queue *q, FILE *fs) {

    // Print the headers
    fprintf(fs, "%6s %-30s %-13s %-13s %-13s\n",
            "PID", "COMMAND", "ARRIVAL", "START", "END");
    
    Process *current = q->head;
    while (current) {
        fprintf(fs, "%6d %-30s %-13.2f %-13.2f %-13.2f\n", 
                current->pid, current->command, current->arrival_time, 
                current->start_time, current->end_time);
        current = current->next;
    }
}

/**
 * Clear all the elements from a queue, freeing the associated memory.
 * @param q     Queue structure.
 **/
void queue_clear(Queue *q) {
    Process *current;
    while (q->head) {
        current = queue_pop(q);
        process_delete(current);
    }
}
/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
