add_executable(image-transform ${sources}
        main.c
        bmp_writer.c
        bmp_writer.h
        ray_marching.c
        ray_marching.h
        objects.c
        vec3.c
        vec3.h
        object.h
        scene.c
        scene.h)
target_include_directories(image-transform PRIVATE src include)

target_link_libraries(image-transform m)
