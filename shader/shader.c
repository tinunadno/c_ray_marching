#include <math.h>

#include "../math_additions/vec3.h"
#include "../scene_meta_inf/scene.h"
#include "../object/object.h"
#include "../ray_marching/ray_marching.h"

#define clamp(value, lower, upper) (value > upper ? upper : (value < lower ? lower : value))

struct shader{
    float ambient;
    float alpha;
    float diffuse;
    float specular;
    struct vec3 (*process_shader)(struct vec3* pos, struct vec3* color, struct scene* scene, struct shader* shader);
};

struct vec3 diffuse_and_specular_shader(struct vec3* pos, struct vec3* color, struct scene* scene, struct shader* shader){
    struct vec3 ambient = *color;
    mul_scal_vec3(&ambient, shader->ambient);

    struct vec3 diffusion_and_specular = {0, 0, 0};
    struct vec3 normal = get_normal(pos, scene);

    for (int i = 0; i < scene->light_sources_count; i++) {
        struct vec3 pos_light_vector = scene->light_sources[i];
        sub_vec3(&pos_light_vector, pos);
        normalize(&pos_light_vector);
        float diffusion_value = shader->diffuse * clamp(dot_product(&pos_light_vector, &normal), 0.0f, 1.0f);
        struct vec3 diffusion_color = *color;
        mul_scal_vec3(&diffusion_color, diffusion_value);

        struct vec3 reflect = normal;
        mul_scal_vec3(&reflect, 2.0f * dot_product(&pos_light_vector, &normal));
        sub_vec3(&reflect, &pos_light_vector);
        normalize(&reflect);
        struct vec3 specular = {255, 255, 255};
        struct vec3 viewer_vector = scene->scene_camera->camera_position;
        sub_vec3(&viewer_vector, pos);
        normalize(&viewer_vector);
        float specular_value =
                shader->specular * (powf(clamp(dot_product(&reflect, &viewer_vector), 0.0f, 1.0f), shader->alpha));
        mul_scal_vec3(&specular, specular_value);

        add_vec3(&diffusion_and_specular, &diffusion_color);
        add_vec3(&diffusion_and_specular, &specular);
    }

    add_vec3(&ambient, &diffusion_and_specular);
    return ambient;
}