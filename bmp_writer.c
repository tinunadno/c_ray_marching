#include <stdint-gcc.h>
#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#define CALCULATE_PADDING(rowSize) ((4 - ((rowSize) % 4)) % 4)

struct __attribute__((packed)) bmp_header {
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bfOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};


int write_bmp_file(char *output_file_path, uint8_t* screen, size_t x_size, size_t y_size) {
    FILE *file = fopen(output_file_path, "wb");
    //checking if file opened normally
    if (!file) {
        perror("Failed to open file for writing");
        return 1;
    }

    size_t padding = CALCULATE_PADDING(x_size*3);
    size_t row_size = (x_size * 3 + padding);
    size_t total_size = row_size * y_size;


    struct bmp_header header = {0};
    header.bfType = 0x4D42;
    header.bfileSize = total_size + sizeof(struct bmp_header);
    header.bfOffBits = sizeof(struct bmp_header);
    header.biSize = 40;
    header.biWidth = x_size;
    header.biHeight = y_size;
    header.biPlanes = 1;
    header.biBitCount = 24;
    header.biCompression = 0;
    header.biSizeImage = total_size;


    if(fwrite(&header,  sizeof(struct bmp_header), 1, file)!=1){
        free(screen);
        fclose(file);
        return 1;
    }

    for(size_t i = 0;i<y_size;i++){
        if(fwrite(screen+x_size*3*i, sizeof(uint8_t), x_size*3, file)!=x_size*3){
            free(screen);
            fclose(file);
            return 1;
        }
        uint8_t padding_bytes[3] = {0};
        if (padding > 0 && fwrite(padding_bytes, 1, padding, file) != padding) {
            perror("Failed to write padding");
            fclose(file);
            return 1;
        }
    }

    free(screen);
    fclose(file);

    return 0;
}