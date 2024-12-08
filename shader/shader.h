//
// Created by yura on 12/8/24.
//

#ifndef TEMP_SHADER_H
#define TEMP_SHADER_H

struct shader{
    float ambient;
    float alpha;
    float diffuse;
    float specular;
    struct vec3 (*process_shader)(struct vec3* pos, struct vec3* color, struct scene* scene, struct shader* shader);
};

struct vec3 diffuse_and_specular_shader(struct vec3* pos, struct vec3* color, struct scene* scene, struct shader* shader);
#endif //TEMP_SHADER_H
