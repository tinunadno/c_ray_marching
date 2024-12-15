//
// Created by yura on 12/7/24.
//

#ifndef TEMP_OBJECT_H
#define TEMP_OBJECT_H

#include <stdbool.h>
#include "../math_additions/vec3.h"
#include "../some_structures/linked_list.h"
struct object {
    struct vec3 position;
    struct vec3 color;
    float size;
    float (*map)(struct vec3 *obj_pos, struct vec3 *ray_pos, float size);
    struct shader *shader;
    struct rotation* rotation;
};

void destroy_object(struct object* obj, struct node* freed_pointers);

struct object_relationship {
    struct object *objects;
    int object_count;

    float (*map)(struct vec3 *ray_pos, struct object *objects, int object_count);

    struct vec3 (*process_shaders)(struct vec3 *pos, struct scene *scene, int relation_index);
};

void destroy_object_relationship(struct object_relationship* or, struct node* freed_pointers);

float sphere_map(struct vec3* obj_pos, struct vec3* ray_pos, float size);

float cube_map(struct vec3* obj_pos, struct vec3* ray_pos, float size);

float surface_map(struct vec3* obj_pos, struct vec3* ray_pos, float size);

float merge_map(struct vec3* ray_pos, struct object* objects, int object_count);

float intersect_map(struct vec3 *ray_pos, struct object *objects, int object_count);

float union_map(struct vec3 *ray_pos, struct object *objects, int object_count);

float difference_map(struct vec3 *ray_pos, struct object *objects, int object_count);

struct vec3 merge_shader(struct vec3 *ray_pos, struct scene *scene, int relation_index);

struct vec3 intersect_shader(struct vec3 *ray_pos, struct scene *scene, int relation_index);

#endif //TEMP_OBJECT_H
