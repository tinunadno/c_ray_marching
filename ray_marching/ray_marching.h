//
// Created by yura on 12/2/24.
//

#ifndef TEMP_RAY_MARCHING_H
#define TEMP_RAY_MARCHING_H

#include <stddef.h>
#include "../object/object.h"
#include "../scene_meta_inf/scene.h"

struct vec3 get_color(struct vec3 *vector, struct scene* scene);

struct vec3 get_normal(struct vec3 *vector, struct scene *scene);

struct ray_march_return{
    bool is_crossed;
    struct vec3 ray;
};

struct ray_march_return march_ray(struct vec3* position, struct vec3* rotation, struct scene *scene);

struct vec3 render_pixel(struct vec3* uv, struct scene* scene);
void asd();
#endif //TEMP_RAY_MARCHING_H
