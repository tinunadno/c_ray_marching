#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include "../math_additions/vec3.h"
#include "../some_structures/linked_list.h"
#include "../string_additions/string_additions.h"
#include "../scene_meta_inf/scene.h"
#include "../shader/shader.h"
#include "../rotation_stuff/rotation.h"
#include "../object/object.h"

struct xml_tree{
    char* tag;
    char* data;
    bool is_leaf;
    struct node* children;
};

struct field_meta_description{
    char* field_name;
    bool is_array;
    size_t field_size;
    size_t displacement;
    union {
        void (*primitive_handler)(const char *data, void *field_address);
        void (*non_primitive_parser)(struct xml_tree* xml_tree, struct field_meta_description* array_description, void *field_address);
    };
    struct field_meta_description* inner_structures;
};

struct available_functions{
    char* function_name;
    void* function_pointer;
};

struct available_enums{
    char* enum_name;
    uint32_t enum_value;
};

struct available_functions available_functions[] = {
        {"diffuse_and_specular", diffuse_and_specular_shader},
        {"cube", cube_map},
        {"sphere", sphere_map},
        {"surface", surface_map},
        {"merge_map", merge_map},
        {"merge_shader", merge_shader},
        {NULL, NULL}
};

struct available_enums available_enums[] = {
        {"quaternion", QUATERNION_ROTATION},
        {"matrix", MATRIX_ROTATION},
        {NULL, 0}
};

struct xml_tree* find_node_by_tag(struct xml_tree* parent_node, char* tag){
    if(compare_strings(parent_node->tag, tag)){
        return parent_node;
    }
    struct node* children = parent_node->children;
    if(!children){
        return NULL;
    }
    while(children){
        struct xml_tree* current_node = (struct xml_tree*)children->data;
        if(compare_strings(current_node->tag, tag)){
            return current_node;
        }
        children = children->next;
    }
    return NULL;
}

void parse_vec3(const char* description, void* field_address){
    struct vec3 ret;
    if (sscanf(description, "%f,%f,%f", &ret.x, &ret.y, &ret.z) != 3) {
        fprintf(stderr, "error in vec3 definition: %s\n", description);
        exit(EXIT_FAILURE);
    }
    *(struct vec3*)field_address = ret;
}

void parse_float(const char* description, void* field_address){
    float value;
    if (sscanf(description, "%f", &value) != 1) {
        fprintf(stderr, "error in float definition: %s\n", description);
        exit(EXIT_FAILURE);
    }
    *(float*)field_address = value;
}
void parse_function(const char* description, void* field_address){
    for(int i = 0; available_functions[i].function_name != NULL; i++){
        //TODO replace char* with const char* in string compare
        if(compare_strings(description, available_functions[i].function_name)){
            *(void**)field_address = available_functions[i].function_pointer;
            return;
        }
    }
    fprintf(stderr, "can't find function with name '%s', check available functions description", description);
    exit(EXIT_FAILURE);
}
void parse_enum(const char* description, void* field_address){
    for(int i = 0; available_enums[i].enum_value != NULL; i++){
        //TODO replace char* with const char* in string compare
        if(compare_strings(description, available_enums[i].enum_name)){
            *(uint32_t *)field_address = available_enums[i].enum_value;
            return;
        }
    }
    fprintf(stderr, "enum type '%s' is invalid", description);
    exit(EXIT_FAILURE);
}

void parse_struct(struct xml_tree* xml_tree, void* structure, struct field_meta_description* field_meta_description){
    for(int i = 0; field_meta_description[i].field_name != NULL; i++){
        struct xml_tree* current_field_node = find_node_by_tag(xml_tree, field_meta_description[i].field_name);
        if(!current_field_node){
            fprintf(stderr, "field %s not defined in xml confing", field_meta_description[i].field_name);
            exit(EXIT_FAILURE);
        }
        if(field_meta_description[i].is_array){
            field_meta_description[i].non_primitive_parser(current_field_node, &field_meta_description[i], (char*)structure);
        }else if(field_meta_description[i].inner_structures != NULL){
            void* inner_struct = malloc(field_meta_description[i].field_size);
            if(!inner_struct){
                fprintf(stderr, "can't allocate memory for inner structure %s", field_meta_description[i].field_name);
                exit(EXIT_FAILURE);
            }
            parse_struct(current_field_node, inner_struct, field_meta_description[i].inner_structures);
            void* field_address = (char*)structure + field_meta_description[i].displacement;
            *(void**)field_address = inner_struct;
        }else{
            field_meta_description[i].primitive_handler(current_field_node->data, structure + field_meta_description[i].displacement);
        }
    }
}

void parse_array(struct xml_tree* xml_tree, struct field_meta_description* array_description, void *field_address){
    struct node* children = xml_tree->children;
    uint32_t children_amount = get_linked_list_node_amount(children);
    *(uint32_t *)(field_address + array_description->inner_structures->displacement) = children_amount;
    *(void**)(field_address + array_description->displacement) = malloc(array_description->field_size * children_amount);
    uint32_t i = 0;
    void *array_base = *(void **)(field_address + array_description->displacement);
    while(children!=NULL){
        parse_struct((struct xml_tree*)children->data, (char*)array_base + (array_description->field_size * i++), array_description->inner_structures->inner_structures);
        children = children->next;
    }
}

struct scene* convert_xml_tree_to_scene(struct xml_tree* xml_tree){
    struct field_meta_description camera_fields[] = {
            {"camera-position", false,sizeof(struct vec3), offsetof(struct camera, camera_position), parse_vec3, NULL},
            {"camera-rotation", false,sizeof(struct vec3), offsetof(struct camera, camera_rotation), parse_vec3, NULL},
            {NULL, false,0, 0, NULL, NULL}
    };
    struct field_meta_description shader_fields[] = {
            {"ambient", false,sizeof(float), offsetof(struct shader, ambient), parse_float, NULL},
            {"alpha", false,sizeof(float), offsetof(struct shader, alpha), parse_float, NULL},
            {"diffuse", false,sizeof(float), offsetof(struct shader, diffuse), parse_float, NULL},
            {"specular", false,sizeof(float), offsetof(struct shader, specular), parse_float, NULL},
            {"process_shader", false,sizeof(struct vec3 (*)(struct vec3* pos, struct vec3* color, struct scene* scene, struct shader* shader)), offsetof(struct shader, process_shader), parse_function, NULL},
            {NULL, false,0, 0, NULL, NULL},
    };
    struct field_meta_description rotation_fields[] = {
            {"rotation-angles", false,sizeof(struct vec3), offsetof(struct rotation, rotation_angles), parse_vec3, NULL},
            {"rotation-type", false,sizeof(enum rotation_type), offsetof(struct rotation, rotation_type), parse_enum, NULL},
            {NULL, false,0, 0, NULL, NULL}
    };
    struct field_meta_description object_fields[] = {
            {"position", false,sizeof(struct vec3), offsetof(struct object, position), parse_vec3, NULL},
            {"color", false,sizeof(struct vec3), offsetof(struct object, color), parse_vec3, NULL},
            {"size", false,sizeof(float), offsetof(struct object, size), parse_float, NULL},
            {"map", false,sizeof(float (*)(struct object* obj, struct vec3* ray_pos)), offsetof(struct object, map), parse_function, NULL},
            {"shader", false,sizeof(struct shader*), offsetof(struct object, shader), 0, shader_fields},
            {"rotation", false,sizeof(struct rotation*), offsetof(struct object, rotation), 0, rotation_fields},
            {NULL, false,0, 0, NULL, NULL}
    };
    struct field_meta_description objects_array_description = {"objects_count", false, sizeof(int), offsetof(struct object_relationship, object_count), NULL, object_fields};
    struct field_meta_description object_relationship_fields[] = {
            {"objects", true, sizeof(struct object), offsetof(struct object_relationship, objects), .non_primitive_parser = parse_array, &objects_array_description},
            {"map", false, sizeof(float (*)(struct vec3 *ray_pos, struct object *objects, int object_count)), offsetof(struct object_relationship, map), parse_function, NULL},
            {"shader-processing", false, sizeof(struct vec3 (*)(struct vec3 *pos, struct scene *scene, int relation_index)), offsetof(struct object_relationship, process_shaders), parse_function, NULL},
            {NULL, false,0, 0, NULL, NULL}
    };
    struct field_meta_description light_sources_field[] = {
            {"light-source", false, sizeof(struct vec3), 0, parse_vec3, NULL},
            {NULL, false,0, 0, NULL, NULL}};
    struct field_meta_description light_sources_array_description[] = {"light_sources_count", false, sizeof(int),offsetof(struct scene, light_sources_count), NULL, &light_sources_field};
    struct field_meta_description scene_objects_array_description = {"scene_objects_count", false, sizeof(int),offsetof(struct scene, scene_objects_count), NULL, object_fields};
    struct field_meta_description scene_relations_array_description = {"object_relationship_count", false, sizeof(int),offsetof(struct scene, object_relations_count), NULL, object_relationship_fields};
    struct field_meta_description scene_fields[] = {
            {"camera", false, sizeof(struct camera*), offsetof(struct scene, scene_camera), 0, camera_fields},
            {"light-sources", true, sizeof(struct vec3), offsetof(struct scene, light_sources), .non_primitive_parser = parse_array, &light_sources_array_description},
            {"scene-objects", true, sizeof(struct object), offsetof(struct scene, scene_objects), .non_primitive_parser = parse_array, &scene_objects_array_description},
            {"object-relations", true, sizeof(struct object_relationship), offsetof(struct scene, object_relations), .non_primitive_parser = parse_array, &scene_relations_array_description},
            {NULL, false,0, 0, NULL, NULL}

    };

    struct scene* scene = malloc(sizeof(struct scene));
    parse_struct((struct xml_tree*)xml_tree->children->data, scene, scene_fields);
    return scene;
}

