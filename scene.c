#include <malloc.h>
#include "vec3.h"
#include "object.h"

struct scene{
    struct camera* scene_camera;
    struct vec3* light_sources;
    int light_sources_count;
    struct object* scene_objects;
    int scene_objects_count;
    struct object_system* scene_object_systems;
    int scene_object_systems_count;
};

struct camera{
    struct vec3 camera_position;
    struct vec3 camera_rotation;
};

struct scene* setup_scene_settings(){

    struct scene* scene = malloc(sizeof (struct scene));

    scene->scene_camera = malloc(sizeof(struct camera));

    scene->scene_camera->camera_position = (struct vec3){-10.0f, 3, 2};
    scene->scene_camera->camera_rotation = (struct vec3){-5.0f, 2, 1};

    scene->scene_objects_count = 0;
    scene->light_sources_count = 2;
    scene->scene_object_systems_count = 1;
    scene->scene_objects = malloc(sizeof(struct object)*scene->scene_objects_count);
    scene->scene_object_systems= malloc(sizeof(struct object_system)*scene->scene_object_systems_count);
    scene->light_sources= malloc(sizeof(struct vec3)*scene->light_sources_count);

    scene->scene_object_systems[0].system_objects_count = 1;
    scene->scene_object_systems[0].object_system_map = merge_map;

    scene->scene_object_systems[0].system_objects = malloc(sizeof(struct object)*scene->scene_object_systems[0].system_objects_count);

    scene->scene_object_systems[0].system_objects[0] = (struct object){
            {0, -1.0f, 0},
            {0, 128, 12},
            0.5f,
            1.0f,
            0.5f,
            sphere_map
    };
    scene->scene_object_systems[0].system_objects[1] = (struct object){
            {0, 1.0f, 0},
            {128, 128, 12},
            0.5f,
            1.0f,
            0.5f,
            cube_map
    };


    scene->light_sources[0] = (struct vec3){
            -10.0f, -5.0f, 5.0f
    };
    scene->light_sources[1] = (struct vec3){
            -10.0f, -5.0f, -5.0f
    };
    return scene;
}

void destroy_scene(struct scene* scene){
    free(scene->scene_camera);
    free(scene->light_sources);
    free(scene->scene_objects);
    free(scene);
}