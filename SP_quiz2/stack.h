#ifndef STACK_H
#define STACK_H
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif
#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
struct stack
{
    int count;
    int buf[BUF_SIZE];
};

void push(struct stack *s, int a);
int pop(struct stack *s);
void showstack(struct stack *s);

#endif
