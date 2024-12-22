#include <stdint-gcc.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>

uint32_t find_sub_string_from_index(const char *string, const char *sub_string, uint32_t index, uint32_t start) {
    uint32_t i = start;
    uint32_t sub_string_start_index = 0;
    uint32_t appear_index = 0;
    while (string[i] != '\0') {
        sub_string_start_index = i;
        uint32_t j = 0;
        bool flag = true;
        while (sub_string[j] != '\0') {
            if (string[i++] != sub_string[j++]) {
                flag = false;
                break;
            }
        }
        if (flag && appear_index++ == index) {
            return sub_string_start_index;
        }
    }
    return -1;
}

uint32_t find_sub_string(const char *string, const char *sub_string, uint32_t index) {
    return find_sub_string_from_index(string, sub_string, index, 0);
}

bool contains_sub_string(const char *string, const char *sub_string) {
    return find_sub_string(string, sub_string, 0) != -1;
}

uint32_t get_string_length(const char *string) {
    uint32_t i = 0;
    while (string[i++] != '\0');
    return --i;
}

char *get_sub_string(const char *string, uint32_t start, uint32_t end) {
    if(get_string_length(string) == 0){
        return NULL;
    }
    char *sub_string = malloc(sizeof(char) * (end - start) + 1);
    if (!sub_string) {
        perror("failed to allocate memory for substring");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = start; i < end; i++) {
        sub_string[i - start] = string[i];
    }
    sub_string[end - start] = '\0';
    return sub_string;
}

char *concatenate_strings(const char *fst_str, const char *snd_str) {
    uint32_t first_size = get_string_length(fst_str);
    uint32_t second_size = get_string_length(snd_str);
    char *concatenated_string = malloc(first_size + second_size + 1);
    if (!concatenated_string) {
        perror("failed to allocate memory for concatenation");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < first_size; i++) {
        concatenated_string[i] = fst_str[i];
    }
    for (int j = 0; j < second_size; j++) {
        concatenated_string[first_size + j] = snd_str[j];
    }
    concatenated_string[first_size + second_size] = '\0';
    return concatenated_string;
}

static char *
concatenate_for_replace(char *string, uint32_t old_index, uint32_t old_length, char *new, uint32_t string_length,
                        uint32_t new_length) {
    uint32_t new_string_length = string_length - old_length + new_length;
    char *ret = malloc(new_string_length + 1);
    if (!ret) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    memcpy(ret, string, old_index);
    memcpy(ret + old_index, new, new_length);
    memcpy(ret + old_index + new_length, string + old_index + old_length, string_length - old_index - old_length);
    ret[new_string_length] = '\0';
    return ret;
}

static void replace_first_sub_string(char **string, char *old, char *new, uint32_t replace_index, uint32_t old_length,
                                     uint32_t new_length) {
    uint32_t string_length = strlen(*string);
    char *replaced = concatenate_for_replace(*string, replace_index, old_length, new, string_length, new_length);
    char *temp = *string;
    *string = replaced;
    free(temp);
}

void replace_sub_string(char **string, char *old, char *new) {
    if (!string || !*string || !old || !new) {
        fprintf(stderr, "Null pointer passed to replace_sub_string\n");
        exit(EXIT_FAILURE);
    }
    uint32_t old_length = strlen(old);
    uint32_t new_length = strlen(new);
    uint32_t replace_index = find_sub_string(*string, old, 0);
    while (replace_index != -1) {
        replace_first_sub_string(string, old, new, replace_index, old_length, new_length);
        replace_index = find_sub_string_from_index(*string, old, 0, replace_index);
    }
}

void string_to_lower_case(char *string) {
    uint32_t i = 0;
    while(string[i] != '\0'){
        if(string[i] >= 0x41 && string[i] <= 0x5a){
            string[i] += (0x61 - 0x41);
        }
        i++;
    }
}

bool compare_strings(char* fst_string, char* snd_string){
    uint32_t i = 0;
    while(fst_string[i]!='\0' && snd_string[i]!='\0'){
        if(fst_string[i] != snd_string[i]){
            return false;
        }
        i++;
    }
    return true;
}

char* mul_string(char* string, uint32_t repeat_amount){
    uint32_t string_length = get_string_length(string);
    char* ret = malloc(string_length*repeat_amount + 1);
    for(int i = 0; i < string_length*repeat_amount; i++){
        ret[i] = string[i%string_length];
    }
    ret[string_length*repeat_amount] = '\0';
    return ret;
}

char* copy_string(char* string){
    uint32_t string_length = get_string_length(string) + 1;
    char* ret = malloc(string_length);
    if(!ret){
        perror("failed to allocate memory for string copy");
        exit(EXIT_FAILURE);
    }
    memcpy(ret, string, string_length);
    return ret;
}

void remove_white_spaces(char **string) {
    replace_sub_string(string, "\n", "");
    replace_sub_string(string, " ", "");
    replace_sub_string(string, "\t", "");
}