#include <stdio.h>
#include <stdint-gcc.h>
#include <stdlib.h>
#include <memory.h>
#include "../scene_meta_inf/scene.h"
#include "../string_additions/string_additions.h"
#include "../some_structures/linked_list.h"

struct xml_tree{
    char* tag;
    char* data;
    bool is_leaf;
    struct node* children;
};

static char* build_tag(char* tag_body, bool is_closed){
    char* first_half = concatenate_strings((is_closed ? "</" : "<"), tag_body);
    char* full_tag = concatenate_strings(first_half, ">");
    free(first_half);
    return full_tag;
}

static char* get_tag_body(char* xml){
    return get_sub_string(xml,
                          find_sub_string(xml, "<", 0) + 1,
                          find_sub_string(xml, ">", 0));
}

static char* get_tag_middle(char* xml, char* tag){
    char* full_tag_closed = build_tag(tag, true);
    char* full_tag_opened = build_tag(tag, false);

    uint32_t tag_start = find_sub_string(xml, full_tag_opened, 0);
    uint32_t tag_end = find_sub_string(xml, full_tag_closed, 0);
    uint32_t tag_start_length = get_string_length(full_tag_opened);

    free(full_tag_opened);
    free(full_tag_closed);

    return get_sub_string(xml, tag_start + tag_start_length, tag_end);
}

static struct xml_tree* parse_xml(char* xml_file) {
    struct xml_tree* ret = malloc(sizeof(struct xml_tree));
    if (!ret) {
        perror("Failed to allocate memory for xml_tree");
        exit(EXIT_FAILURE);
    }

    char* tag = get_tag_body(xml_file);
    if (!tag) {
        fprintf(stderr, "Failed to parse root tag\n");
        free(ret);
        return NULL;
    }

    ret->tag = copy_string("root");
    ret->data = NULL;
    ret->is_leaf = false;
    ret->children = NULL;

    size_t i = 0;
    while (1) {
        char* closed_tag = build_tag(tag, true);
        i = find_sub_string(xml_file, closed_tag, 0);
        if (i == -1) {
            free(closed_tag);
            break;
        }

        char* middle = get_tag_middle(xml_file, tag);
        if (!middle) {
            free(closed_tag);
            break;
        }

        if (!contains_sub_string(middle, "<")) {
            struct xml_tree* child = malloc(sizeof(struct xml_tree));
            if (!child) {
                perror("Failed to allocate memory for xml_leaf");
                exit(EXIT_FAILURE);
            }
            child->tag = copy_string(tag);
            child->data = copy_string(middle);
            child->is_leaf = true;
            child->children = NULL;

            if (!ret->children) {
                ret->children = create_node((size_t)(void*)child);
            } else {
                add_element(&ret->children, (size_t)(void*)child);
            }

            free(middle);
        } else {
            struct xml_tree* child = parse_xml(middle);
            if (child) {
                child->tag = copy_string(tag);
                if (!ret->children) {
                    ret->children = create_node((size_t)(void*)child);
                } else {
                    add_element(&ret->children, (size_t)(void*)child);
                }
            }

            free(middle);
        }

        char* temp = xml_file + i + get_string_length(closed_tag);
        xml_file = temp;

        free(tag);
        tag = get_tag_body(xml_file);
        if (!tag) {
            free(closed_tag);
            break;
        }
        free(closed_tag);
    }

    return ret;
}


static void print_xml_tree(struct xml_tree* xml_tree, uint32_t rec_depth){
    char* tabulation = mul_string("\t", rec_depth);
    if(!xml_tree){
        return;
    }
    if(xml_tree->is_leaf){
        printf("%s", tabulation);
        printf("%s\n", xml_tree->tag);
        printf("%s", tabulation);
        printf("\t%s\n", xml_tree->data);
        free(tabulation);
        return;
    }
    if(!xml_tree->children){
        free(tabulation);
        return;
    }
    struct node* current_child = xml_tree->children;
    printf("%s", tabulation);
    printf("%s\n", xml_tree->tag);
    while(current_child!=NULL){
        print_xml_tree((struct xml_tree*)(void*)(current_child->data), rec_depth+1);
        current_child = current_child->next;
    }
    free(tabulation);
}

static void free_xml_tree(struct xml_tree* xml_tree){
    if(!xml_tree){
        return;
    }
    if(xml_tree->is_leaf){
        free(xml_tree->tag);
        free(xml_tree->data);
        free(xml_tree);
        return;
    }
    struct node* current_child = xml_tree->children;
    while(current_child != NULL){
        free_xml_tree((struct xml_tree*)(void*)current_child->data);
        current_child = current_child->next;
    }
    free_list(&xml_tree->children);
    free(xml_tree);
}

struct scene *parse_scene(char *xml_file_path) {

    FILE* xml_file = fopen(xml_file_path, "rb");
    if(!xml_file){
        perror("failed to open xml file");
        exit(EXIT_FAILURE);
    }
    char *xml;
    long file_size;

    fseek(xml_file, 0l, SEEK_END);
    file_size = ftell(xml_file);
    rewind(xml_file);
    xml = malloc(sizeof(char) * file_size + 1);
    if (xml == NULL) {
        fclose(xml_file);
        perror("can't allocate memory for xml file");
        exit(EXIT_FAILURE);
    }
    xml[file_size] = '\0';
    if (fread(xml, file_size, 1, xml_file) == -1) {
        free(xml);
        fclose(xml_file);
    }

    remove_white_spaces(&xml);
    string_to_lower_case(xml);
    struct xml_tree* xml_tree = parse_xml(xml);
    free(xml_file);
    print_xml_tree(xml_tree, 0);
    free_xml_tree(xml_tree);
    //TODO add convertion from xml_tree to struct scene
    return NULL;
}