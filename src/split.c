#include "global.h"

void split_file(char *inputfilename, const char *outputfilename) 
{
    FILE *file = fopen(inputfilename, "rb");
    uint8_t buffer[encode_block_size*8];
    if (!file) {
        color_printf(_ERROR_, "split: Unable to open file for reading.\n");
        return;
    }

    // 分割文件
    for (int i = 0; i < encode_block_cnt; i++) {
        char output_filename_cnt[256];
        snprintf(output_filename_cnt, sizeof(output_filename_cnt), "%s_%d", outputfilename, i);
        FILE *output_file = fopen(output_filename_cnt, "wb");
        if (!output_file) {
            color_printf(_ERROR_, "split: Unable to create output file %s\n",output_filename_cnt);
            fclose(file);
            return;
        }

        size_t bytes_read = fread(buffer, 1, encode_block_size, file);  // 从源文件读取数据
        fwrite(buffer, 1, bytes_read, output_file);  // 将数据写入新的文件

        // 刷新缓冲区确保数据被写入磁盘
        if (fflush(output_file) != 0) {
           color_printf(_ERROR_, "split: fail to fflush output file\n");
        } 
        fclose(output_file);
    }

    fclose(file);
    color_printf(_PASS_, "split: success to split input file\n");
}
