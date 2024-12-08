//
// Created by yura on 12/7/24.
//

#ifndef TEMP_OBJECT_H
#define TEMP_OBJECT_H
#include "vec3.h"
struct object{
    struct vec3 position;
    struct vec3 color;
    float diffuse;
    float size;
    float specular;
    float (*map)(struct vec3* obj_pos, struct vec3* ray_pos, float size);
};

struct object_system{
    struct object* system_objects;
    int system_objects_count;
    float (*object_system_map)(struct vec3* ray_pos, struct object_system* object_system);
};

float sphere_map(struct vec3* obj_pos, struct vec3* ray_pos, float size);

float cube_map(struct vec3* obj_pos, struct vec3* ray_pos, float size);

float merge_map(struct vec3* ray_pos, struct object_system* object_system);
#endif //TEMP_OBJECT_H
