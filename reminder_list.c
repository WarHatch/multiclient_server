#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

typedef struct node {
    char name[50];
    time_t time;
    char* details; //Shouldn't be longer than 1000 chars

    struct node * next;
} reminder_t;

int lastElementIndex (reminder_t *head)
{
    int index = -1;

    reminder_t *current = head;

    while (current != NULL) {
        index++;
        current = current->next;
    }

    printf("%d elements in DS\n", index+1);
    return index;
}

reminder_t * getLastElement (reminder_t *head)
{
    reminder_t *last = head;

    while (last->next != NULL) {
        last = last->next;
    }

    return last;
}

reminder_t *create (char name[50], time_t time, char* details){
    //Allocates space for pointers and basic types
    reminder_t* new = malloc(sizeof(struct node));

    //Must allocate memory for contents of pointers.  Here, strdup() creates a new copy of details
    strcpy(new->name, name);
    memcpy(&(new->time), &time, sizeof(time_t));
    new->details = strdup(details);
    new->next = NULL;

    return new;
}

reminder_t * addReminder (reminder_t *head, char name[50], time_t time, char* details)
//returns last elements pointer
{
    printf("Called 'addReminder' func. for %s\n", name);
    
    reminder_t * last = getLastElement(head);
    //last element in the list is assigned

    last->next = create(name, time, details);

    return last->next;
}


int main(int argc, char *argv[]){
    reminder_t* first = create("John", time(0), "Oh look at these details");
    reminder_t *last = addReminder(first, "Grandma's party", time(0), "SHOTS SHOTS SHOTS!");
    last = addReminder(first, "Kise", time(0), "Can't wait to get my krabai from her. She's pretty awesome!");
    last = addReminder(first, "Four", time(0), "");
    last = addReminder(first, "Five", time(0), "");
    printf("last element: *value->next = %d, name = %s, det. = %s\n", last->next, last->name, last->details);
    lastElementIndex(first);

    printf("Program has successfully finished working.\n");
    return 0;
}