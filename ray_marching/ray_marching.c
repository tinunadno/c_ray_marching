#include <math.h>
#include <stdint-gcc.h>
#include <time.h>
#include <stdio.h>

#include "../math_additions/vec3.h"
#include "../object/object.h"
#include "../scene_meta_inf/scene.h"
#include "../shader/shader.h"

#define MAX_ITERATION_COUNT 200
#define LOWER_TRASH_HOLD 0.001f
#define UPPER_TRASH_HOLD 50000
#define NORMAL_TRASH_HOLD 0.00001f

float map(struct vec3 *vector, struct scene *scene) {
    float min_map = UPPER_TRASH_HOLD;
    for (int i = 0; i < scene->scene_objects_count; i++) {
        //float (*map)(struct vec3* obj_pos, struct vec3* ray_pos, float size)
        float current_map = scene->scene_objects[i].map(&scene->scene_objects[i], vector);
        if (current_map < min_map) {
            min_map = current_map;
        }
    }
    for (int i = 0; i < scene->object_relations_count; i++) {
        float current_map = scene->object_relations[i].map(vector, scene->object_relations[i].objects,
                                                           scene->object_relations[i].object_count);
        if (current_map < min_map) {
            min_map = current_map;
        }
    }
    return min_map;
}

struct vec3 get_color(struct vec3 *vector, struct scene *scene) {
    float min_map = UPPER_TRASH_HOLD;
    struct vec3 color = {0, 0, 0};
    for (int i = 0; i < scene->scene_objects_count; i++) {
        //float (*map)(struct vec3* obj_pos, struct vec3* ray_pos, float size)
        float current_map = scene->scene_objects[i].map(&scene->scene_objects[i], vector);
        if (current_map < min_map) {
            min_map = current_map;
            //struct vec3 (*process_shader)(struct vec3* pos, struct vec3* color, struct scene* scene, struct shader* shader);
            color = scene->scene_objects[i].shader->process_shader(vector, &scene->scene_objects[i].color, scene,
                                                                   scene->scene_objects[i].shader);
        }
    }
    for (int i = 0; i < scene->object_relations_count; i++) {
        float current_map = scene->object_relations[i].map(vector, scene->object_relations[i].objects,
                                                           scene->object_relations[i].object_count);
        if (current_map < min_map) {
            min_map = current_map;
            //struct vec3 merge_shader(struct vec3 *ray_pos, struct scene *scene, int relation_index)
            color = scene->object_relations[i].process_shaders(vector, scene, i);
        }
    }
    return color;
}

struct vec3 get_normal(struct vec3 *vector, struct scene *scene) {
    struct vec3 ret;
    ret.x = map(&(struct vec3) {vector->x + NORMAL_TRASH_HOLD, vector->y, vector->z}, scene) -
            map(&(struct vec3) {vector->x - NORMAL_TRASH_HOLD, vector->y, vector->z}, scene);
    ret.y = map(&(struct vec3) {vector->x, vector->y + NORMAL_TRASH_HOLD, vector->z}, scene) -
            map(&(struct vec3) {vector->x, vector->y - NORMAL_TRASH_HOLD, vector->z}, scene);
    ret.z = map(&(struct vec3) {vector->x, vector->y, vector->z + NORMAL_TRASH_HOLD}, scene) -
            map(&(struct vec3) {vector->x, vector->y, vector->z - NORMAL_TRASH_HOLD}, scene);
    normalize(&ret);
    return ret;
}

struct ray_march_return{
  bool is_crossed;
  struct vec3 ray;
};

long long unsigned int map_only =0;
struct ray_march_return march_ray(struct vec3* position, struct vec3* rotation, struct scene *scene) {

    struct vec3 local_rotation = *rotation;
    struct vec3 ray = *position;
    normalize(&local_rotation);

    add_vec3(&ray, &local_rotation);
    float distance = map(&ray, scene);

    float total_distance = distance;
    struct vec3 step = local_rotation;
    mul_scal_vec3(&step, total_distance);

    int k = 0;
    while (k++ < MAX_ITERATION_COUNT && total_distance < UPPER_TRASH_HOLD && distance > LOWER_TRASH_HOLD ) {
        step = local_rotation;
        mul_scal_vec3(&step, total_distance);
        ray = *position;
        add_vec3(&ray, &step);
        uint64_t diff;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        distance = map(&ray, scene);
        clock_gettime(CLOCK_MONOTONIC, &end);       /* mark the end time */

        diff = 1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
        map_only+=diff;
        total_distance+=distance;
    }

    return (struct ray_march_return){
            distance <= LOWER_TRASH_HOLD,
            ray
    };
}


long long unsigned int total_diff_marching =0;
long long unsigned int total_diff_shadering =0;

struct vec3 render_pixel(struct vec3* uv, struct scene* scene){

    uint64_t diff;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);     /* mark start time */


    struct vec3 rotation = scene->scene_camera->camera_rotation;
    sub_vec3(&rotation, &scene->scene_camera->camera_position);
    add_vec3(&rotation, uv);



    struct ray_march_return result = march_ray(&scene->scene_camera->camera_position, &rotation, scene);

    clock_gettime(CLOCK_MONOTONIC, &end);       /* mark the end time */

    diff = 1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    total_diff_marching+=diff;


    struct vec3 temp;
    clock_gettime(CLOCK_MONOTONIC, &start);     /* mark start time */
    if(result.is_crossed){
        temp = get_color(&result.ray, scene);
    }else{
        temp = (struct vec3){
            0,0,0
        };
    }
    clock_gettime(CLOCK_MONOTONIC, &end);       /* mark the end time */

    diff = 1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    total_diff_shadering+=diff;
    return temp;
}

void asd(){
    printf("elapsed time for marching = %f seconds\n", (double) total_diff_marching/1000000000.0);
    printf("elapsed time for shading = %f seconds\n", (double) total_diff_shadering/1000000000.0);
    printf("elapsed time for mapping = %f seconds\n", (double) map_only/1000000000.0);
}