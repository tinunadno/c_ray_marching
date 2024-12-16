#include <math.h>
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

void calculate_matrix_rotation_matrix(struct rotation *mr) {
    if (!mr || (mr->rotation_angles.x == 0 && mr->rotation_angles.y == 0 && mr->rotation_angles.z == 0) ||
        mr->rotation_type == QUATERNION_ROTATION) {
        return;
    }
    float cos_alpha = cosf(mr->rotation_angles.x);
    float sin_alpha = sinf(mr->rotation_angles.x);
    float cos_beta = cosf(mr->rotation_angles.y);
    float sin_beta = sinf(mr->rotation_angles.y);
    float cos_gamma = cosf(mr->rotation_angles.z);
    float sin_gamma = sinf(mr->rotation_angles.z);

    mr->rotation_matrix[0] = cos_beta * cos_gamma;
    mr->rotation_matrix[1] = -cos_beta * sin_gamma;
    mr->rotation_matrix[2] = sin_beta;

    mr->rotation_matrix[3] = cos_alpha * sin_gamma + cos_gamma * sin_alpha * sin_beta;
    mr->rotation_matrix[4] = cos_alpha * cos_gamma - sin_alpha * sin_beta * sin_gamma;
    mr->rotation_matrix[5] = -cos_beta * sin_alpha;

    mr->rotation_matrix[6] = sin_alpha * sin_gamma - cos_alpha * cos_gamma * sin_beta;
    mr->rotation_matrix[7] = cos_gamma * sin_alpha + cos_alpha * sin_beta * sin_gamma;
    mr->rotation_matrix[8] = cos_alpha * cos_beta;
}

static struct vec4 set_quaternion_angle(float angle, float x, float y, float z) {
    float half_angle = angle / 2.0f;
    float half_angle_cosine = cosf(half_angle);
    return (struct vec4) {sinf(half_angle), half_angle_cosine * x, half_angle_cosine * y, half_angle_cosine * z};
}

static struct vec4 multiply_quaternion(struct vec4 *q1, struct vec4 *q2) {
    return (struct vec4) {
            .x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y,
            .y = q1->w * q2->y - q1->x * q2->z + q1->y * q2->w + q1->z * q2->x,
            .z = q1->w * q2->z + q1->x * q2->y - q1->y * q2->x + q1->z * q2->w,
            .w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z
    };
}

static struct vec4 conjugate_quaternion(struct vec4 *q) {
    return (struct vec4) {q->x, -q->y, -q->z, -q->w};
}

void calculate_quaternion_rotation(struct rotation *mr) {
    if (!mr || (mr->rotation_angles.x == 0 && mr->rotation_angles.y == 0 && mr->rotation_angles.z == 0) ||
        mr->rotation_type == MATRIX_ROTATION) {
        return;
    }
    struct vec4 q_x = set_quaternion_angle(mr->rotation_angles.x, 1, 0, 0);
    struct vec4 q_y = set_quaternion_angle(mr->rotation_angles.y, 0, 1, 0);
    struct vec4 q_z = set_quaternion_angle(mr->rotation_angles.z, 0, 0, 1);
    struct vec4 rotation_quaternion = multiply_quaternion(&q_x, &q_y);
    rotation_quaternion = multiply_quaternion(&rotation_quaternion, &q_z);
    mr->quaternion[0] = rotation_quaternion.x;
    mr->quaternion[1] = rotation_quaternion.y;
    mr->quaternion[2] = rotation_quaternion.z;
    mr->quaternion[3] = rotation_quaternion.w;
}

void apply_matrix_rotation(struct vec3 *vector, struct rotation *mr) {
    if (!mr || mr->rotation_angles.x == 0 && mr->rotation_angles.y == 0 && mr->rotation_angles.z == 0 ||
        mr->rotation_type == QUATERNION_ROTATION) {
        return;
    }
    struct vec3 ret = {0, 0, 0};
    ret.x = mr->rotation_matrix[0] * vector->x + mr->rotation_matrix[1] * vector->y +
            mr->rotation_matrix[2] * vector->z;
    ret.y = mr->rotation_matrix[3] * vector->x + mr->rotation_matrix[4] * vector->y +
            mr->rotation_matrix[5] * vector->z;
    ret.z = mr->rotation_matrix[6] * vector->x + mr->rotation_matrix[7] * vector->y +
            mr->rotation_matrix[8] * vector->z;
    *vector = ret;
}

void apply_quaternion_rotation(struct vec3 *vector, struct rotation *mr) {
    if (!mr || mr->rotation_angles.x == 0 && mr->rotation_angles.y == 0 && mr->rotation_angles.z == 0 ||
        mr->rotation_type == MATRIX_ROTATION) {
        return;
    }
    struct vec4 rotated = {0, vector->x, vector->y, vector->z};
    rotated = multiply_quaternion((struct vec4 *) mr->quaternion, &rotated);
    struct vec4 conj = conjugate_quaternion((struct vec4 *) mr->quaternion);
    rotated = multiply_quaternion(&rotated, &conj);
    vector->x = rotated.y;
    vector->y = rotated.z;
    vector->z = rotated.w;
}