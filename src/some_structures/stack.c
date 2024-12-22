#include <stddef.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint-gcc.h>

struct stack{
    size_t data;
    uint32_t cell_number;
    struct stack* prev;
};

struct stack* create_stack_instance(size_t data){
    struct stack* stack = malloc(sizeof(struct stack));
    if(!stack){
        perror("failed to allocate memory for stack");
        exit(EXIT_FAILURE);
    }
    stack->data = data;
    stack->cell_number = 0;
    stack->prev = NULL;
    return stack;
}

void push_to_stack(struct stack** last, size_t data){
    struct stack* stack = create_stack_instance(data);
    stack->prev = *last;
    stack->cell_number = (*last)->cell_number+1;
    *last = stack;
}

size_t pop_from_stack(struct stack** last){
    size_t ret = (*last)->data;
    struct stack* removal = *last;
    *last = (*last)->prev;
    free(removal);
    return ret;
}

void free_stack_as_pointer_set(struct stack* last){
    while(last!=NULL){
        struct stack* temp = last;
        last = last->prev;
        free((void*)temp->data);
        free(temp);
    }
}

void free_stack(struct stack* last){
    while(last!=NULL){
        struct stack* temp = last;
        last = last->prev;
        free(temp);
    }
}