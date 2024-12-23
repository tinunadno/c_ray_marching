//
// Created by yura on 12/15/24.
//

#ifndef TEMP_LINKED_LIST_H
#define TEMP_LINKED_LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdint-gcc.h>

struct node {
    size_t data;
    struct node *next;
};

struct node *create_node(size_t data);

void add_element_to_linked_list(struct node **head, size_t data);

bool linked_list_contains_element(struct node* head, size_t data);

uint32_t get_linked_list_node_amount(struct node* head);

void free_linked_list(struct node** head);
#endif //TEMP_LINKED_LIST_H
