#include <math.h>
#include "vec3.h"
#define min(a, b) (a<=b ? a : b)
#define max(a, b) (a>=b ? a : b)
#define clamp(value, lower, upper) (value > upper ? upper : (value < lower ? lower : value))

struct object{
    struct vec3 position;
    struct vec3 color;
    float size;
    float diffuse;
    float specular;
    float (*map)(struct vec3* obj_pos, struct vec3* ray_pos, float size);
};

struct object_system{
    struct object* system_objects;
    int system_objects_count;
    float (*object_system_map)(struct vec3* ray_pos, struct object_system* object_system);
};

float sphere_map(struct vec3* obj_pos, struct vec3* ray_pos, float size) {
    struct vec3 local_pos = *ray_pos;
    sub_vec3(&local_pos, obj_pos);
    return get_len(&local_pos) - size;
}

float cube_map(struct vec3* obj_pos, struct vec3* ray_pos, float size) {
    struct vec3 local_pos = *ray_pos;
    sub_vec3(&local_pos, obj_pos);
    vec3_abs(&local_pos);
    sub_scalar(&local_pos, size);
    float max_dist = max(local_pos.x, max(local_pos.y, local_pos.z));
    float clamped_dist = max(max_dist, 0.0f);

    float min_dist = min(max_dist, 0.0f);

    return sqrtf(clamped_dist * clamped_dist + min_dist * min_dist);
}

static float smin(float a, float b, float k){
    float h = clamp(0.5f+0.5f*(b-a)/k, 0.0f, 1.0f);
    return min(b, min(a, h)) - k*h*(1.0f-h);
}

float merge_map(struct vec3* ray_pos, struct object_system* object_system){
    struct object* current_object = object_system->system_objects;
    float smooth_min = current_object->map(&current_object->position, ray_pos, current_object->size);
    float k = 0.1f;
    for(int i=0;i<object_system->system_objects_count;i++){
        current_object+=sizeof(struct object);
        smooth_min = smin(smooth_min, current_object->map(&current_object->position, ray_pos, current_object->size), k);
    }
    return smooth_min;
}