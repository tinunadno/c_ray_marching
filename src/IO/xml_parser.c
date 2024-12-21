#include <bits/types/FILE.h>
#include <stdio.h>
#include <malloc.h>
#include <stdint-gcc.h>
#include <stdlib.h>
#include "../scene_meta_inf/scene.h"
#include "../string_additions/string_additions.h"

static void* parse_xml(char* xml_file){
    replace_sub_string(&xml_file, "\n", "");
    replace_sub_string(&xml_file, "\t", "");
    replace_sub_string(&xml_file, " ", "");

//    char* tag = get_sub_string(xml_file,
//                               find_sub_string(xml_file, "<", 0)+1,
//                               find_sub_string(xml_file, ">", 0));
//    char* full_tag = get_sub_string(xml_file,
//                                    find_sub_string(xml_file, "<", 0),
//                                    find_sub_string(xml_file, ">", 0) + 1);
//    char* closed_tag_tmp = concatenate_strings("</", tag);
//    char* closed_tag = concatenate_strings(closed_tag_tmp, ">");
//    uint32_t tag_close_index = find_sub_string(xml_file, closed_tag, 0);
//
//    char* middle = get_sub_string(xml_file, find_sub_string(xml_file, full_tag, 0) + get_string_length(full_tag), tag_close_index);
//
//    free(tag);
//    free(full_tag);
//    free(middle);
//    free(closed_tag_tmp);
//    free(closed_tag);

    printf("%s", xml_file);

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

    parse_xml(xml);

    return 0;
}