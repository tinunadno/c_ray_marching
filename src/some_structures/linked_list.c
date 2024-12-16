#include <stddef.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>

struct node {
    size_t data;
    struct node *next;
};

struct node *create_node(size_t data) {
    struct node *new_node = malloc(sizeof(struct node));
    if (!new_node) {
        perror("Failed to allocate memory for a new node");
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void add_element(struct node **head, size_t data) {
    struct node *new_node = create_node(data);
    struct node *temp = *head;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = new_node;
}

bool contains_element(struct node* head, size_t data) {
    struct node* temp = head;
    while (temp) {
        if (temp->data == data) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void free_list(struct node** head) {
    struct node* current = *head;
    struct node* next;
    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}