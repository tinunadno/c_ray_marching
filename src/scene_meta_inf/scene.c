#include <malloc.h>
#include <stdbool.h>
#include "../math_additions/vec3.h"
#include "../object/object.h"
#include "../shader/shader.h"
#include "../rotation_stuff/rotation.h"
#include "../some_structures/linked_list.h"

#define PI4th 0.78539816339f

struct scene {
    struct camera *scene_camera;
    struct vec3 *light_sources;
    int light_sources_count;
    struct object *scene_objects;
    int scene_objects_count;
    struct object_relationship *object_relations;
    int object_relations_count;
};

struct camera {
    struct vec3 camera_position;
    struct vec3 camera_rotation;
};

struct scene *setup_scene_settings() {

    struct scene *scene = malloc(sizeof(struct scene));

    scene->scene_camera = malloc(sizeof(struct camera));

    scene->scene_camera->camera_position = (struct vec3) {-10.0f, 0, 2};
    scene->scene_camera->camera_rotation = (struct vec3) {-5.0f, 0, 1};

    scene->scene_objects_count = 0;
    scene->object_relations_count = 1;
    scene->light_sources_count = 3;
    if(scene->scene_objects_count!=0)
        scene->scene_objects = malloc(sizeof(struct object) * scene->scene_objects_count);
    if(scene->object_relations_count != 0 )
        scene->object_relations = malloc(sizeof(struct object_relationship) * scene->object_relations_count);
    scene->light_sources = malloc(sizeof(struct vec3) * scene->light_sources_count);

    scene->object_relations[0].object_count = 3;

    scene->object_relations[0].objects = malloc(sizeof(struct object) * scene->object_relations[0].object_count);

    scene->object_relations[0].map = merge_map;
    scene->object_relations[0].process_shaders = merge_shader;

    struct shader *cube_shader = malloc(sizeof(struct shader));
    *cube_shader = (struct shader) {
            0.4f,
            4.0f,
            0.5f,
            0.5f,
            diffuse_and_specular_shader
    };

    struct rotation *cube_rotation = malloc(sizeof(struct rotation));
    *cube_rotation = (struct rotation) {
            .rotation_angles = (struct vec3) {PI4th, 0, 0},
            .rotation_type = QUATERNION_ROTATION,
    };

    scene->object_relations[0].objects[0] = (struct object) {
            {0, -.8f, 0.2f},
            {128, 0, 12},
            1.0f,
            cube_map,
            cube_shader,
            cube_rotation
    };
    scene->object_relations[0].objects[1] = (struct object) {
            {-1, 0.0f, 0},
            {100, 0, 128},
            1.0f,
            sphere_map,
            cube_shader,
            0
    };
    scene->object_relations[0].objects[2] = (struct object) {
            {-1.0f, 1.2f, 0.2f},
            {0, 128, 12},
            1.5f,
            sphere_map,
            cube_shader,
            0
    };


    scene->light_sources[0] = (struct vec3) {
            5.0f, 5.0f, 5.0f
    };
    scene->light_sources[1] = (struct vec3) {
            5.0f, -5.0f, 5.0f
    };
    scene->light_sources[2] = (struct vec3) {
            -5.0f, -5.0f, -5.0f
    };
    return scene;
}

void calculate_rotations(struct scene *scene) {
    for (int i = 0; i < scene->scene_objects_count; i++) {
        if(scene->scene_objects[i].rotation!=NULL) {
            if (scene->scene_objects[i].rotation->rotation_type == QUATERNION_ROTATION) {
                scene->scene_objects[i].rotation->calculate_rotation = calculate_quaternion_rotation;
                scene->scene_objects[i].rotation->apply_rotation = apply_quaternion_rotation;
            } else if (scene->scene_objects[i].rotation->rotation_type == MATRIX_ROTATION) {
                scene->scene_objects[i].rotation->calculate_rotation = calculate_matrix_rotation_matrix;
                scene->scene_objects[i].rotation->apply_rotation = apply_matrix_rotation;
            }
            scene->scene_objects[i].rotation->calculate_rotation(scene->scene_objects[i].rotation);
        }
    }
    for (int i = 0; i < scene->object_relations_count; i++) {
        for (int j = 0; j < scene->object_relations[i].object_count; j++) {
            if(scene->object_relations[i].objects[j].rotation != NULL) {
                if (scene->object_relations[i].objects[j].rotation->rotation_type == QUATERNION_ROTATION) {
                    scene->object_relations[i].objects[j].rotation->calculate_rotation = calculate_quaternion_rotation;
                    scene->object_relations[i].objects[j].rotation->apply_rotation = apply_quaternion_rotation;
                } else if (scene->object_relations[i].objects[j].rotation->rotation_type == MATRIX_ROTATION) {
                    scene->object_relations[i].objects[j].rotation->calculate_rotation = calculate_matrix_rotation_matrix;
                    scene->object_relations[i].objects[j].rotation->apply_rotation = apply_matrix_rotation;
                }
                scene->object_relations[i].objects[j].rotation->calculate_rotation(
                        scene->object_relations[i].objects[j].rotation);
            }
        }
    }
}

void destroy_scene(struct scene *scene) {
    free(scene->scene_camera);
    free(scene->light_sources);
    struct node *freed_pointers = create_node(0);
    for (int i = 0; i < scene->scene_objects_count; i++) {
        destroy_object(&scene->scene_objects[i], freed_pointers);
    }
    if (scene->scene_objects_count != 0) {
        free(scene->scene_objects);
    }
    for (int i = 0; i < scene->object_relations_count; i++) {
        destroy_object_relationship(&scene->object_relations[i], freed_pointers);
    }
    if(scene->object_relations_count!=0) {
        free(scene->object_relations);
    }
    free_linked_list(&freed_pointers);
    free(scene);
}