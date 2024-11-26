#include "stack.h"  
void push(struct stack *s, int a)
{
    if(s->count < BUF_SIZE)
        s->buf[s->count++] = a;
    else
        printf("Not have enough space in this stack\n");
}

int pop(struct stack *s)
{
    if(s->count > 0)
        return s->buf[--s->count];
    else
        printf("Nothing in this stack\n");
    return -100;
}

void showstack(struct stack *s)
{
    printf("\n* Data in stack: ");
    for(int i = 0; i < s->count; i++)
        printf("%d ", s->buf[i]);
    printf("\n");
}

