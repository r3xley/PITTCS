/* >​< >‌‌‌<
 * Developed by R. E. Bryant, 2017
 * Extended to store strings, 2018
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    /* What if malloc returned NULL? */
    if (q == NULL)
    {
        return NULL;
    }
    q->head = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* How about freeing the list elements and the strings? */
    /* Free queue structure */
    if (q == NULL)
    {
        return; // nothing
    }

    list_ele_t *temp = q->head;
    while (temp != NULL)
    {
        list_ele_t *curr = temp;
        temp = temp->next;

        free(curr->value); // Free the string
        free(curr);        // Free the element or node
    }

    free(q);
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{

    /* What should you do if the q is NULL? */
    if (q == NULL)
    {
        return false;
    }
    list_ele_t *newh;
    int size_string = strlen(s) + 1;
    char *string_copy;

    string_copy = malloc(size_string * sizeof(char));
    // if malloc fails
    if (string_copy == NULL)
    {
        return false;
    }

    newh = malloc(sizeof(list_ele_t));

    if (newh == NULL)
    {
        // if fails need to free the memory from above
        free(string_copy);
        return false;
    }
    strcpy(string_copy, s);

    newh->value = string_copy;
    // handle empty queue
    if (q->head == NULL)
    {
        q->tail = newh;
        newh->next = NULL;
    }
    /* Don't forget to allocate space for the string and copy it */
    /* What if either call to malloc returns NULL? */
    /* You must do the cleanup of anything left behind */
    newh->next = q->head;
    q->head = newh;
    q->size++;
    return true;
}

/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{

    /*Handle queue not-existing situation*/
    if (q == NULL)
    {
        return false;
    }

    list_ele_t *newt;
    int size_string = strlen(s) + 1;
    char *string_copy;

    newt = malloc(sizeof(list_ele_t));

    if (newt == NULL)
    {

        return false;
    }

    string_copy = malloc(size_string * sizeof(char));
    /*Handle malloc failure*/
    if (string_copy == NULL)
    {
        free(newt);
        return false;
    }

    // allocate space and check if null

    /* You need to write the complete code for this function */
    /* Remember: It should operate in O(1) time */

    strcpy(string_copy, s);
    newt->value = string_copy;
    newt->next = NULL;
    // handle the empty queue
    if (q->head == NULL)
    {
        q->head = newt;
        q->tail = newt;

        return true;
    }
    q->tail->next = newt;
    q->tail = newt;
    q->size++;
    return true;
}

/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If sp is non-NULL and an element is removed, copy the removed string to *sp
  (up to a maximum of bufsize-1 characters, plus a null terminator.)
  The space used by the list element and the string should be freed.
*/
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    // base case
    if (q == NULL || q->head == NULL)
        return false;
    // If sp is not null copy
    // max copied size is buf - 1
    if (sp != NULL)
    {
        strncpy(sp, q->head->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    // free the space
    list_ele_t *temp = q->head;

    q->head = q->head->next;

    free(temp->value);
    free(temp);
    q->size--;
    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    if (q == NULL)
    {
        return 0;
    }
    return q->size;
}

/*
  Reverse elements in queue
  No effect if q is NULL or empty
  This function should not allocate or free any list elements
  (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
  It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* You need to write the code for this function */
    // reverse a linked list
    if (q == NULL || q->head == NULL)
        return;
    // use temp variable
    list_ele_t *temp, *prev, *next;
    prev = q->head;
    temp = prev->next;
    next = temp;
    q->head->next = NULL;
    q->tail = q->head;
    while (temp != NULL)
    {
        next = temp->next;
        temp->next = prev;
        prev = temp;
        temp = next;
    }
    q->head = prev;
    return;
}
