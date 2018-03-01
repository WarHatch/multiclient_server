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

reminder_t *create (char name[50], time_t time, char* details){
    //Allocates space for pointes and basic types
    reminder_t* new = malloc(sizeof(struct node));

    //Must allocate memory for contents of pointers.  Here, strdup() creates a new copy of name
    strcpy(new->name, name);
    memcpy(&(new->time), &time, sizeof(time_t));
    new->details = strdup(details);

    return new;
}


int main(int argc, char *argv[]){
    //GET time for testing
    time_t rawtime = time(0);

    reminder_t* first = create("John", rawtime, "Oh look at these details");
    reminder_t* second = create("Grandma's party", time(0), "SHOTS SHOTS SHOTS!");

    //CONVERT time to readable text
    struct tm* timeinfo;
    //time ( &(first->time) );
    timeinfo = localtime ( &(first->time) );

    printf("*The created struct is made of:\n%s , %s , %s \n", first->name, asctime (timeinfo), first->details);
    printf("*The second struct is made of:\n%s , %s , %s \n", second->name, asctime (localtime ( &(second->time) )), second->details);
    
    first->next = second;

    printf("**The second item in LL:\n%s , %s , %s \n", first->next->name, asctime (localtime ( &(first->next->time) )), first->next->details);

    return 0;
}

/* creation
node_t * head = NULL;
head = malloc(sizeof(node_t));
if (head == NULL) {
    return 1;
}

head->val = 1;
head->next = NULL;
*/