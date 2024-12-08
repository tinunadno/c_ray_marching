//
// Created by yura on 12/2/24.
//

#ifndef TEMP_RAY_MARCHING_H
#define TEMP_RAY_MARCHING_H

#include <stddef.h>
#include "object.h"
#include "scene.h"

struct vec3 march_ray(struct vec3* uv, struct scene* scene);
#endif //TEMP_RAY_MARCHING_H
