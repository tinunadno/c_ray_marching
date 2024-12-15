
#ifndef TEMP_VEC3_H
#define TEMP_VEC3_H
struct vec3 {
    float x, y, z;
};

struct vec4 {
    float x, y, z, w;
};

void add_vec3(struct vec3 *fst_vec, struct vec3 *snd_vec);

void sub_vec3(struct vec3 *fst_vec, struct vec3 *snd_vec);

void add_scalar(struct vec3 *vector, float val);

void sub_scalar(struct vec3 *vector, float val);

void mul_scal_vec3(struct vec3 *vector, float val);

void div_scal_vec3(struct vec3 *vector, float val);

void mul_vec3(struct vec3 *fst_vec, struct vec3 *snd_vec);

void div_vec3(struct vec3 *fst_vec, struct vec3 *snd_vec);

float get_len(struct vec3 *vec);

void normalize(struct vec3 *vec);

void vec3_abs(struct vec3 *vec);

void clamp_vec3_lower_bound(struct vec3 *vec, float lower_value);

float dot_product(struct vec3 *a, struct vec3 *b);

#endif //TEMP_VEC3_H
