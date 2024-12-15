//
// Created by yura on 12/15/24.
//

#ifndef TEMP_ROTATION_H
#define TEMP_ROTATION_H

#include "../math_additions/vec3.h"

enum rotation_type {
    MATRIX_ROTATION = 1,
    QUATERNION_ROTATION = 2
};

struct rotation {
    struct vec3 rotation_angles;
    enum rotation_type rotation_type;
    union {
        float rotation_matrix[9];
        float quaternion[4];
    };

    void (*calculate_rotation)(struct rotation *mr);

    void (*apply_rotation)(struct vec3 *vector, struct rotation *mr);
};

void calculate_matrix_rotation_matrix(struct rotation *mr);

void calculate_quaternion_rotation(struct rotation *mr);

void apply_matrix_rotation(struct vec3 *vector, struct rotation *mr);

void apply_quaternion_rotation(struct vec3 *vector, struct rotation *mr);

#endif //TEMP_ROTATION_H
