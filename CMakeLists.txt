add_executable(image-transform ${sources}
        main.c
        IO/bmp_writer.c
        IO/bmp_writer.h
        ray_marching/ray_marching.c
        ray_marching/ray_marching.h
        object/objects.c
        math_additions/vec3.c
        math_additions/vec3.h
        object/object.h
        scene_meta_inf/scene.c
        scene_meta_inf/scene.h
        shader/shader.c
        shader/shader.h
        shader/shader.c
        rotation_stuff/rotation.c
        rotation_stuff/rotation.h
        some_structures/linked_list.c
        some_structures/linked_list.h)
target_include_directories(image-transform PRIVATE src include)

target_link_libraries(image-transform m)
