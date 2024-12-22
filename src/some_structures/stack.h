#ifndef SRC_RAY_MARCHING_STACK_H
#define SRC_RAY_MARCHING_STACK_H

#include <stddef.h>
#include <stdint-gcc.h>

struct stack{
    size_t data;
    uint32_t cell_number;
    struct stack* prev;
};

struct stack* create_stack_instance(size_t data);

void push_to_stack(struct stack** last, size_t data);

size_t pop_from_stack(struct stack** last);

void free_stack(struct stack* last);
#endif //SRC_RAY_MARCHING_STACK_H
