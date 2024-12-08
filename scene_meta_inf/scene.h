//
// Created by yura on 12/8/24.
//

#ifndef TEMP_SCENE_H
#define TEMP_SCENE_H
#include "../math_additions/vec3.h"

struct scene{
    struct camera* scene_camera;
    struct vec3* light_sources;
    int light_sources_count;
    struct object* scene_objects;
    int scene_objects_count;
    struct object_relationship* object_relations;
    int object_relations_count;
};

struct camera{
    struct vec3 camera_position;
    struct vec3 camera_rotation;
};

struct scene* setup_scene_settings();

void destroy_scene(struct scene* scene);
#endif //TEMP_SCENE_H
