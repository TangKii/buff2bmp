#include "global.h"

void merge_file(char *inputfilename, const char *outputfilename)
{
    FILE *file = fopen(outputfilename, "wb");
    uint8_t buffer[encode_block_size*8];

    uint8_t *y_data = (unsigned char *)malloc(image_width*image_height);
    uint8_t * u_data = (unsigned char *)malloc(image_width*image_height / 2);
    uint8_t * v_data = (unsigned char *)malloc(image_width*image_height / 2);

    if (!file) {
        printf("merge: Unable to open file for writing.\n");
        return;
    }

    // 合并文件
    for (int i = 0; i < encode_block_cnt; i++) {
        char input_filename_cnt[256];
        snprintf(input_filename_cnt, sizeof(input_filename_cnt), "%s_%d.yuv", inputfilename, i);
        FILE *output_file = fopen(input_filename_cnt, "rb");
        if (!output_file) {
            printf("merge: Unable to open input file.\n");
            fclose(file);
            return;
        }

        fread(y_data + (encode_block_size * radio/2*i), sizeof(uint8_t), encode_block_size * radio/2, output_file);
        fread(u_data + (encode_block_size * radio/4*i), sizeof(uint8_t), encode_block_size * radio/4 , output_file);
        fread(v_data + (encode_block_size * radio/4*i), sizeof(uint8_t), encode_block_size * radio/4, output_file);
        fclose(output_file);

    }

    fwrite(y_data, 1, image_width*image_height, file);  // 将数据写入新的文件
    fwrite(u_data, 1, image_width*image_height/2, file);  // 将数据写入新的文件
    fwrite(v_data, 1, image_width*image_height/2, file);  // 将数据写入新的文件

    // 刷新缓冲区确保数据被写入磁盘
    if (fflush(file) != 0) {
        printf("merge: fail to fflush output file\n");
    } 
    fclose(file);
}
