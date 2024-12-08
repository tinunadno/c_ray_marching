#include <math.h>

#define abs(val) (val < 0.0f ? val*(-1.0f) : val)
struct vec3 {
    float x, y, z;
};

void add_vec3(struct vec3 *fst_vec, struct vec3 *snd_vec) {
    fst_vec->x += snd_vec->x;
    fst_vec->y += snd_vec->y;
    fst_vec->z += snd_vec->z;
}

void sub_vec3(struct vec3 *fst_vec, struct vec3 *snd_vec) {
    fst_vec->x -= snd_vec->x;
    fst_vec->y -= snd_vec->y;
    fst_vec->z -= snd_vec->z;
}

void add_scalar(struct vec3 *vector, float val) {
    vector->x += val;
    vector->y += val;
    vector->z += val;
}

void sub_scalar(struct vec3 *vector, float val) {
    vector->x -= val;
    vector->y -= val;
    vector->z -= val;
}

void mul_scal_vec3(struct vec3 *vector, float val) {
    vector->x *= val;
    vector->y *= val;
    vector->z *= val;
}

void div_scal_vec3(struct vec3 *vector, float val) {
    vector->x /= val;
    vector->y /= val;
    vector->z /= val;
}

void mul_vec3(struct vec3 *fst_vec, struct vec3 *snd_vec) {
    fst_vec->x *= snd_vec->x;
    fst_vec->y *= snd_vec->y;
    fst_vec->z *= snd_vec->z;
}

void div_vec3(struct vec3 *fst_vec, struct vec3 *snd_vec) {
    fst_vec->x /= snd_vec->x;
    fst_vec->y /= snd_vec->y;
    fst_vec->z /= snd_vec->z;
}

float get_len(struct vec3 *vec) {
    return sqrtf(vec->x * vec->x +
                 vec->y * vec->y +
                 vec->z * vec->z);
}

void normalize(struct vec3 *vec) {
    float len = get_len(vec);
    vec->x /= len;
    vec->y /= len;
    vec->z /= len;
}

void vec3_abs(struct vec3 *vec) {
    vec->x = abs(vec->x);
    vec->y = abs(vec->y);
    vec->z = abs(vec->z);
}

void clamp_vec3_lower_bound(struct vec3 *vec, float lower_value) {
    vec->x = (vec->x < lower_value ? lower_value : vec->x);
    vec->y = (vec->y < lower_value ? lower_value : vec->y);
    vec->z = (vec->z < lower_value ? lower_value : vec->z);
}

float dot_product(struct vec3 *a, struct vec3 *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}