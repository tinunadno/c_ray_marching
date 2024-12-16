
project(ray_marching)

set(sources
        main.c
        IO/bmp_writer.c
        ray_marching/ray_marching.c
        object/objects.c
        math_additions/vec3.c
        scene_meta_inf/scene.c
        shader/shader.c
        rotation_stuff/rotation.c
        some_structures/linked_list.c
)

add_executable(ray-marching ${sources})

target_link_libraries(ray-marching m)