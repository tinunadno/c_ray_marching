//
// Created by yura on 12/15/24.
//

#ifndef TEMP_LINKED_LIST_H
#define TEMP_LINKED_LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>

struct node {
    size_t data;
    struct node *next;
};

struct node *create_node(size_t data);

void add_element(struct node **head, size_t data);

bool contains_element(struct node* head, size_t data);

void free_list(struct node** head);
#endif //TEMP_LINKED_LIST_H
