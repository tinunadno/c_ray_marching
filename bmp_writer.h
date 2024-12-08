//
// Created by yura on 12/2/24.
//

#ifndef TEMP_BMP_WRITER_H
#define TEMP_BMP_WRITER_H

#include <stdint-gcc.h>
#include <stddef.h>

int write_bmp_file(char *output_file_path, uint8_t* screen, size_t x_size, size_t y_size);
#endif //TEMP_BMP_WRITER_H
