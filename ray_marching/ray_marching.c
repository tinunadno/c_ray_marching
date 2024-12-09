#include <math.h>

#include "../math_additions/vec3.h"
#include "../object/object.h"
#include "../scene_meta_inf/scene.h"
#include "../shader/shader.h"

#define MAX_ITERATION_COUNT 1000
#define LOWER_TRASH_HOLD 0.0001f
#define UPPER_TRASH_HOLD 500000
#define NORMAL_TRASH_HOLD 0.00001f

float map(struct vec3 *vector, struct scene *scene) {
    float min_map = UPPER_TRASH_HOLD;
    for (int i = 0; i < scene->scene_objects_count; i++) {
        //float (*map)(struct vec3* obj_pos, struct vec3* ray_pos, float size)
        float current_map = scene->scene_objects[i].map(&scene->scene_objects[i].position, vector,
                                                        scene->scene_objects[i].size);
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
        float current_map = scene->scene_objects[i].map(&scene->scene_objects[i].position, vector,
                                                        scene->scene_objects[i].size);
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

struct ray_march_return march_ray(struct vec3* position, struct vec3* rotation, struct scene *scene) {

    struct vec3 local_rotation = *rotation;
    int k = 0;
    struct vec3 ray = *position;
    add_vec3(&ray, &local_rotation);
    float distance = map(&ray, scene);
    struct vec3 prev_rotation = local_rotation;
    while (k++ < MAX_ITERATION_COUNT && distance < UPPER_TRASH_HOLD && distance > LOWER_TRASH_HOLD) {
        normalize(&local_rotation);
        mul_scal_vec3(&local_rotation, distance);
        add_vec3(&local_rotation, &prev_rotation);
        prev_rotation = local_rotation;
        ray = scene->scene_camera->camera_position;
        add_vec3(&ray, &local_rotation);
        distance = map(&ray, scene);
    }

    return (struct ray_march_return){
            distance <= LOWER_TRASH_HOLD,
            ray
    };
}

struct vec3 render_pixel(struct vec3* uv, struct scene* scene){
    struct vec3 rotation = scene->scene_camera->camera_rotation;
    sub_vec3(&rotation, &scene->scene_camera->camera_position);
    add_vec3(&rotation, uv);
    normalize(&rotation);

    struct ray_march_return result = march_ray(&scene->scene_camera->camera_position, &rotation, scene);

    if(result.is_crossed){
        return get_color(&result.ray, scene);
    }else{
        return (struct vec3){
            0,0,0
        };
    }
}