#include <math.h>

#include "vec3.h"
#include "object.h"
#include "scene.h"

#define MAX_ITERATION_COUNT 80
#define LOWER_TRASH_HOLD 0.005f
#define UPPER_TRASH_HOLD 100000
#define NORMAL_TRASH_HOLD 0.00001f
#define AMBIENT_VALUE 0.4f
#define DIFFUSE_CONSTANT 0.5f
#define SPECULAR_CONSTANT 0.5f
#define ALPHA 10.0f

#define clamp(value, lower, upper) (value > upper ? upper : (value < lower ? lower : value))

float map(struct vec3 *vector, struct object *objects, int objects_count) {
    float min_map = UPPER_TRASH_HOLD;
    for (int i = 0; i < objects_count; i++) {
        //float (*map)(struct vec3* obj_pos, struct vec3* ray_pos, float size)
        float current_map = objects[i].map(&objects[i].position, vector, objects[i].size);
        if (current_map < min_map) {
            min_map = current_map;
        }
    }
    return min_map;
}

struct vec3 get_color(struct vec3 *vector, struct object *objects, int objects_count) {
    float min_map = UPPER_TRASH_HOLD;
    struct vec3 color = {0, 0, 0};
    for (int i = 0; i < objects_count; i++) {
        //float (*map)(struct vec3* obj_pos, struct vec3* ray_pos, float size)
        float current_map = objects[i].map(&objects[i].position, vector, objects[i].size);
        if (current_map < min_map) {
            min_map = current_map;
            color = objects[i].color;
        }
    }
    return color;
}

struct vec3 get_normal(struct vec3 *vector, struct object *objects, int objects_count) {
    struct vec3 ret;
    ret.x = map(&(struct vec3) {vector->x + NORMAL_TRASH_HOLD, vector->y, vector->z}, objects, objects_count) -
            map(&(struct vec3) {vector->x - NORMAL_TRASH_HOLD, vector->y, vector->z}, objects, objects_count);
    ret.y = map(&(struct vec3) {vector->x, vector->y + NORMAL_TRASH_HOLD, vector->z}, objects, objects_count) -
            map(&(struct vec3) {vector->x, vector->y - NORMAL_TRASH_HOLD, vector->z}, objects, objects_count);
    ret.z = map(&(struct vec3) {vector->x, vector->y, vector->z + NORMAL_TRASH_HOLD}, objects, objects_count) -
            map(&(struct vec3) {vector->x, vector->y, vector->z - NORMAL_TRASH_HOLD}, objects, objects_count);
    normalize(&ret);
    return ret;
}

struct vec3 process_shader(struct vec3 *pos, struct scene* scene) {
    struct vec3 color = get_color(pos, scene->scene_objects, scene->scene_objects_count);
    struct vec3 ambient = color;
    mul_scal_vec3(&ambient, AMBIENT_VALUE);

    struct vec3 diffusion_and_specular = {0,0,0};
    struct vec3 normal = get_normal(pos, scene->scene_objects, scene->scene_objects_count);

    for (int i = 0; i < scene->light_sources_count; i++) {
        struct vec3 pos_light_vector = scene->light_sources[i];
        sub_vec3(&pos_light_vector, pos);
        normalize(&pos_light_vector);
        float diffusion_value = DIFFUSE_CONSTANT * clamp(dot_product(&pos_light_vector, &normal), 0.0f, 1.0f);
        struct vec3 diffusion_color = color;
        mul_scal_vec3(&diffusion_color, diffusion_value);

        struct vec3 reflect = normal;
        mul_scal_vec3(&reflect, 2.0f * dot_product(&pos_light_vector, &normal));
        sub_vec3(&reflect, &pos_light_vector);
        normalize(&reflect);
        struct vec3 specular = {255, 255, 255};
        struct vec3 viewer_vector= scene->scene_camera->camera_position;
        sub_vec3(&viewer_vector, pos);
        normalize(&viewer_vector);
        float specular_value = SPECULAR_CONSTANT * (powf(clamp(dot_product(&reflect, &viewer_vector), 0.0f, 1.0f), ALPHA));
        mul_scal_vec3(&specular, specular_value);

        add_vec3(&diffusion_and_specular, &diffusion_color);
        add_vec3(&diffusion_and_specular, &specular);
    }

    add_vec3(&ambient, &diffusion_and_specular);
    return ambient;
}

struct vec3 march_ray(struct vec3* uv, struct scene* scene) {

    struct vec3 rotation = scene->scene_camera->camera_rotation;
    sub_vec3(&rotation, &scene->scene_camera->camera_position);
    add_vec3(&rotation, uv);
    normalize(&rotation);

    int k = 0;
    struct vec3 ray = scene->scene_camera->camera_position;
    add_vec3(&ray, &rotation);
    float distance = map(&ray, scene->scene_objects, scene->scene_objects_count);
    struct vec3 prev_rotation = rotation;
    while (k++ < MAX_ITERATION_COUNT && distance < UPPER_TRASH_HOLD && distance > LOWER_TRASH_HOLD) {
        normalize(&rotation);
        mul_scal_vec3(&rotation, distance);
        add_vec3(&rotation, &prev_rotation);
        prev_rotation = rotation;
        ray = scene->scene_camera->camera_position;
        add_vec3(&ray, &rotation);
        distance = map(&ray, scene->scene_objects, scene->scene_objects_count);
    }

    if (distance <= LOWER_TRASH_HOLD) {
        return process_shader(&ray, scene);
    } else {
        return (struct vec3) {0.0f, 0.0f, 0.0f};
    }
}