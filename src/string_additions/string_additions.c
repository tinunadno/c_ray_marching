#include <stdint-gcc.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>

uint32_t find_sub_string(const char* string, const char* sub_string, uint32_t index){
    uint32_t i = 0;
    uint32_t sub_string_start_index = 0;
    uint32_t appear_index = 0;
    while(string[i] != '\0'){
        sub_string_start_index = i;
        uint32_t j = 0;
        bool flag = true;
        while(sub_string[j] != '\0'){
            if(string[i++] != sub_string[j++]){
                flag = false;
                break;
            }
        }
        if(flag && appear_index++ == index){
            return sub_string_start_index;
        }
    }
    return -1;
}

bool contains_sub_string(const char* string, const char* sub_string){
    return find_sub_string(string, sub_string, 0) != -1;
}

char* get_sub_string(const char* string, uint32_t start, uint32_t end){
    char* sub_string = malloc(sizeof(char) * (end-start));
    if(!sub_string){
        perror("failed to allocate memory for substring");
        exit(EXIT_FAILURE);
    }
    for(uint32_t i = start; i < end; i++){
        sub_string[i - start] = string[i];
    }
    return sub_string;
}

uint32_t get_string_length(const char* string){
    uint32_t i = 0;
    while(string[i++] != '\0');
    return --i;
}

char* concatenate_strings(const char* fst_str, const char* snd_str){
    uint32_t first_size = get_string_length(fst_str);
    uint32_t second_size = get_string_length(snd_str);
    char* concatenated_string = malloc(sizeof(char)*(first_size + second_size));
    if(!concatenated_string){
        perror("failed to allocate memory for concatenation");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < first_size; i++){
        concatenated_string[i] = fst_str[i];
    }
    for(int j = 0; j <second_size; j++){
        concatenated_string[first_size + j] = snd_str[j];
    }
    return concatenated_string;
}

static char* concatenate_for_replace(char* string, uint32_t old_index, uint32_t old_length, char* new, uint32_t string_length, uint32_t new_length) {
    uint32_t new_string_length = string_length - old_length + new_length;
    char* ret = malloc(new_string_length + 1);
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

void replace_sub_string(char** string, char* old, char* new) {
    uint32_t old_length = strlen(old);
    uint32_t new_length = strlen(new);
    char* current = *string;
    uint32_t string_length = strlen(current);
    uint32_t replace_index = find_sub_string(current, old, 0);

    while (replace_index != (uint32_t)-1) {
        char* temp = concatenate_for_replace(current, replace_index, old_length, new, string_length, new_length);
        free(current);
        current = temp;
        string_length = strlen(current);
        replace_index = find_sub_string(current, old, replace_index + new_length);
    }

    *string = current;
}