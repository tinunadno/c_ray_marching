#include <math.h>
#include <float.h>
#include <malloc.h>
#include "../math_additions/vec3.h"
#include "../scene_meta_inf/scene.h"
#include "../shader/shader.h"
#include "../rotation_stuff/rotation.h"
#include "../some_structures/linked_list.h"

#define min(a, b) (a<=b ? a : b)
#define max(a, b) (a>=b ? a : b)
#define abs(value) (value < 0 ? -value : value)
#define clamp(value, lower, upper) (value > upper ? upper : (value < lower ? lower : value))

struct object {
    struct vec3 position;
    struct vec3 color;
    float size;
    float (*map)(struct object* obj, struct vec3* ray_pos);
    struct shader *shader;
    struct rotation *rotation;
};

void destroy_object(struct object *obj, struct node* freed_pointers) {
    if(!linked_list_contains_element(freed_pointers, (size_t) (void *) obj->shader)) {
        add_element_to_linked_list(&freed_pointers, (size_t) (void *) obj->shader);
        free(obj->shader);
    }
    if (obj->rotation != NULL) {
        if(!linked_list_contains_element(freed_pointers, (size_t) (void *) obj->rotation)) {
            add_element_to_linked_list(&freed_pointers, (size_t) (void *) obj->rotation);
            free(obj->rotation);
        }
    }
}

struct object_relationship {
    struct object *objects;
    int object_count;

    float (*map)(struct vec3 *ray_pos, struct object *objects, int object_count);

    struct vec3 (*process_shaders)(struct vec3 *pos, struct scene *scene, int relation_index);
};

void destroy_object_relationship(struct object_relationship *or, struct node* freed_pointers) {
    for (int i = 0; i < or->object_count; i++) {
        destroy_object(&or->objects[i], freed_pointers);
    }
    free(or->objects);
    free(or);
}

float sphere_map(struct object* obj, struct vec3* ray_pos) {
    struct vec3 local_pos = *ray_pos;
    sub_vec3(&local_pos, &obj->position);
    return get_len(&local_pos) - obj->size;
}

float surface_map(struct object* obj, struct vec3* ray_pos) {
    return ((get_len(ray_pos) > obj->size) ? FLT_MAX : ray_pos->z - obj->position.z);
}

float cube_map(struct object* obj, struct vec3* ray_pos) {
    struct vec3 local_pos = *ray_pos;
    sub_vec3(&local_pos, &obj->position);
    if(obj->rotation != NULL) {
        obj->rotation->apply_rotation(&local_pos, obj->rotation);
    }
    vec3_abs(&local_pos);
    sub_scalar(&local_pos, obj->size);
    float clamped_dist = min(max(local_pos.x, max(local_pos.y, local_pos.z)), 0.0);
    clamp_vec3_lower_bound(&local_pos, 0.0f);
    add_scalar(&local_pos, clamped_dist);
    return get_len(&local_pos);
}

static float smin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return b * (1.0f - h) + a * h - k * h * (1.0f - h);
}

float merge_map(struct vec3 *ray_pos, struct object *objects, int object_count) {
    float k = .3f;
    float smooth_min = objects[0].map(&objects[0], ray_pos);
    for (int i = 1; i < object_count; i++) {
        smooth_min = smin(smooth_min, objects[i].map(&objects[i], ray_pos), k);
    }
    return smooth_min;
}

float intersect_map(struct vec3 *ray_pos, struct object *objects, int object_count) {
    float max_map = objects[0].map(&objects[0], ray_pos);
    for (int i = 1; i < object_count; i++) {
        max_map = max(max_map, objects[i].map(&objects[i], ray_pos));
    }
    return max_map;
}

float union_map(struct vec3 *ray_pos, struct object *objects, int object_count) {
    float min_map = objects[0].map(&objects[0], ray_pos);
    for (int i = 1; i < object_count; i++) {
        min_map = min(min_map, objects[i].map(&objects[i], ray_pos));
    }
    return min_map;
}

float difference_map(struct vec3 *ray_pos, struct object *objects, int object_count) {
    float max_map = objects[0].map(&objects[0], ray_pos);
    for (int i = 1; i < object_count; i++) {
        max_map = max(max_map, -objects[i].map(&objects[i], ray_pos));
    }
    return max_map;
}

struct vec3 merge_shader(struct vec3 *ray_pos, struct scene *scene, int relation_index) {
    float smooth_factor = .3f;

    struct vec3 final_color = {0, 0, 0};
    float total_weight = 0.0f;

    for (int i = 0; i < scene->object_relations[relation_index].object_count; i++) {
        float distance = scene->object_relations[relation_index].objects[i].map(
                &scene->object_relations[relation_index].objects[i], ray_pos);
        float weight = expf(-distance / smooth_factor);
        total_weight += weight;
        struct vec3 object_color = scene->object_relations[relation_index].objects[i].shader->process_shader(
                ray_pos, &scene->object_relations[relation_index].objects[i].color, scene,
                scene->object_relations[relation_index].objects[i].shader);
        final_color.x += object_color.x * weight;
        final_color.y += object_color.y * weight;
        final_color.z += object_color.z * weight;
    }
    if (total_weight > 0.0f) {
        final_color.x /= total_weight;
        final_color.y /= total_weight;
        final_color.z /= total_weight;
    }

    return final_color;
}

struct vec3 intersect_shader(struct vec3 *ray_pos, struct scene *scene, int relation_index) {
    int closest_index = 0;
    float min_dist = FLT_MAX;
    for (int i = 0; i < scene->object_relations[relation_index].object_count; i++) {
        float distance = abs(scene->object_relations[relation_index].objects[i].map(
                &scene->object_relations[relation_index].objects[i], ray_pos));
        if (distance < min_dist) {
            min_dist = distance;
            closest_index = i;
        }
    }
    return scene->object_relations[relation_index].objects[closest_index].shader->process_shader(
            ray_pos, &scene->object_relations[relation_index].objects[closest_index].color, scene,
            scene->object_relations[relation_index].objects[closest_index].shader);
}
