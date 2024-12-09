#include <stdint-gcc.h>
#include <malloc.h>

#include "IO/bmp_writer.h"
#include "ray_marching/ray_marching.h"
#include "math_additions/vec3.h"
#include "object/object.h"
#include "scene_meta_inf/scene.h"

#define clamp(val) (uint8_t)(val < 0 ? 0 : (val>255 ? 255 : val))

int main() {

    int x = 800;
    int y = 400;
    int half_size_x = x / 2;
    int half_size_y = y / 2;
    float ratio = (float) x / (float) y;
    uint8_t *screen = malloc(sizeof(uint8_t) * 3 * x * y);
    int row_size = (x * 3);

    struct scene* scene = setup_scene_settings();



    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            struct vec3 uv = {0, (((float) j - (float) half_size_x) / (float) half_size_x) * ratio,
                              ((float) i - (float) half_size_y) / (float) half_size_y};
            struct vec3 color = render_pixel(&uv, scene);

            screen[row_size * i + j * 3 + 2] = clamp( color.x);
            screen[row_size * i + j * 3 + 1] = clamp( color.y);
            screen[row_size * i + j * 3] = clamp(color.z);
        }
    }
    destroy_scene(scene);
    return write_bmp_file("./output.bmp", screen, x, y);
}