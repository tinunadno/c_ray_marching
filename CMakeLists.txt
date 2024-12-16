
project(src/ray_marching)

set(sources
        src/main.c
        src/IO/bmp_writer.c
        src/ray_marching/ray_marching.c
        src/object/objects.c
        src/math_additions/vec3.c
        src/scene_meta_inf/scene.c
        src/shader/shader.c
        src/rotation_stuff/rotation.c
        src/some_structures/linked_list.c
)

add_executable(ray-marching ${sources})

target_link_libraries(ray-marching m)