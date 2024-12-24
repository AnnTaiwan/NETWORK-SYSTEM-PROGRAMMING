// Improved stack.c for wait/signal mechanism
#include "stack.h"

void errExit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void push(struct stack *s, char a) {
    // Lock the mutex before accessing the stack
    pthread_mutex_lock(&s->mutex);

    while (s->count == STACK_SIZE) {
        printf("\tStack push wait...\n");
        pthread_cond_wait(&s->conditionVar, &s->mutex); 
    }
    // Push the item onto the stack
    printf("\tStack push %c\n", a);
    s->buf[s->count++] = a;
    pthread_cond_signal(&s->conditionVar);
    showstack(s);
    // Unlock the mutex
    pthread_mutex_unlock(&s->mutex);
}

char pop(struct stack *s) {
    char value;
    // Lock the mutex before accessing the stack
    pthread_mutex_lock(&s->mutex);
    // Pop the item from the stack
    while (s->count == 0) {
        printf("\tStack pop wait...\n");
        pthread_cond_wait(&s->conditionVar, &s->mutex); 
    }
    value = s->buf[--s->count];
    printf("\tStack pop %c\n", value);
    pthread_cond_signal(&s->conditionVar);
    showstack(s);
    // Unlock the mutex
    pthread_mutex_unlock(&s->mutex);
    return value;
}

void showstack(struct stack *s) {
    //pthread_mutex_lock(&s->mutex);
    printf("\n\tStack contents: ");
    for (int i = 0; i < s->count; i++) {
        printf("%c ", s->buf[i]);
    }
    printf("\n");
    //pthread_mutex_unlock(&s->mutex);
}

