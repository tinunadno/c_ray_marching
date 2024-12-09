#include <math.h>
#include "../math_additions/vec3.h"
#include "../scene_meta_inf/scene.h"
#include "../shader/shader.h"

#define min(a, b) (a<=b ? a : b)
#define max(a, b) (a>=b ? a : b)
#define clamp(value, lower, upper) (value > upper ? upper : (value < lower ? lower : value))

struct object {
    struct vec3 position;
    struct vec3 color;
    float size;

    float (*map)(struct vec3 *obj_pos, struct vec3 *ray_pos, float size);

    struct shader *shader;
};

struct object_relationship {
    struct object *objects;
    int object_count;

    float (*map)(struct vec3 *ray_pos, struct object *objects, int object_count);

    struct vec3 (*process_shaders)(struct vec3 *pos, struct scene *scene, int relation_index);
};

float sphere_map(struct vec3 *obj_pos, struct vec3 *ray_pos, float size) {
    struct vec3 local_pos = *ray_pos;
    sub_vec3(&local_pos, obj_pos);
    return get_len(&local_pos) - size;
}

float surface_map(struct vec3* obj_pos, struct vec3* ray_pos, float size){
    return ray_pos->z - obj_pos->z;
}

float cube_map(struct vec3 *obj_pos, struct vec3 *ray_pos, float size) {
    struct vec3 local_pos = *ray_pos;
    sub_vec3(&local_pos, obj_pos);
    vec3_abs(&local_pos);
    sub_scalar(&local_pos, size);
    float max_dist = max(local_pos.x, max(local_pos.y, local_pos.z));
    float clamped_dist = max(max_dist, 0.0f);

    float min_dist = min(max_dist, 0.0f);

    return sqrtf(clamped_dist * clamped_dist + min_dist * min_dist);
}

float mix(float a, float b, float k) {
    return a * (1.0f - k) + b * k;
}

static float smin(float a, float b, float k) {
    float h = clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return mix(b, a, h) - k * h * (1.0f - h);
}

float merge_map(struct vec3 *ray_pos, struct object *objects, int object_count) {
    float k = .3f;
    float smooth_min = objects[0].map(&objects[0].position, ray_pos, objects[0].size);
    for (int i = 1; i < object_count; i++) {
        smooth_min = smin(smooth_min, objects[i].map(&objects[i].position, ray_pos, objects[i].size), k);
    }
    return smooth_min;
}


struct vec3 merge_shader(struct vec3 *ray_pos, struct scene *scene, int relation_index) {
    float smooth_factor = .3f;

    struct vec3 final_color = {0, 0, 0};
    float total_weight = 0.0f;

    for (int i = 0; i < scene->object_relations[relation_index].object_count; i++) {
        float distance = scene->object_relations[relation_index].objects[i].map(
                &scene->object_relations[relation_index].objects[i].position, ray_pos,
                scene->object_relations[relation_index].objects[i].size);
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


