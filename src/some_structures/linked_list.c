#include <stddef.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint-gcc.h>

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

void add_element_to_linked_list(struct node **head, size_t data) {
    struct node *new_node = create_node(data);
    struct node *temp = *head;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = new_node;
}

bool linked_list_contains_element(struct node* head, size_t data) {
    struct node* temp = head;
    while (temp) {
        if (temp->data == data) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

uint32_t get_linked_list_node_amount(struct node* head){
    uint32_t ret = 0;
    while(head!=NULL){
        ret++;
        head = head->next;
    }
    return ret;
}

void free_linked_list(struct node** head) {
    struct node* current = *head;
    struct node* next;
    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}