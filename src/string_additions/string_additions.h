//
// Created by yura on 12/21/24.
//

#ifndef SRC_RAY_MARCHING_STRING_ADDITIONS_H
#define SRC_RAY_MARCHING_STRING_ADDITIONS_H

#include <stdint-gcc.h>
#include <stdbool.h>

uint32_t find_sub_string_from_index(const char* string, const char* sub_string, uint32_t index, uint32_t start);

uint32_t find_sub_string(const char* string, const char* sub_string, uint32_t index);

bool contains_sub_string(const char* string, const char* sub_string);

char* get_sub_string(const char* string, uint32_t start, uint32_t end);

uint32_t get_string_length(const char* string);

char* concatenate_strings(const char* fst_str, const char* snd_str);

void replace_sub_string(char** string, char* old, char* new);

void string_to_lower_case(char *string);

bool compare_strings(char* fst_string, char* snd_string);

char* mul_string(char* string, uint32_t repeat_amount);

char* copy_string(char* string);

void remove_white_spaces(char ** string);
#endif //SRC_RAY_MARCHING_STRING_ADDITIONS_H
