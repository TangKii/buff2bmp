#include "global.h"

void cut_w_tail(char *inputfilename, uint32_t input_W, uint32_t input_H, const char *outputfilename,  uint32_t out_W) 
{
    FILE *inputfile = fopen(inputfilename, "rb");
    FILE *output_file = fopen(outputfilename, "wb");

    uint8_t *y_data = (unsigned char *)malloc(input_W*input_H);
    uint8_t * u_data = (unsigned char *)malloc(input_W*input_H / 2);
    uint8_t * v_data = (unsigned char *)malloc(input_W*input_H / 2);

    uint8_t *out_y_data = (unsigned char *)malloc(out_W*input_H);
    uint8_t * out_u_data = (unsigned char *)malloc(out_W*input_H / 2);
    uint8_t * out_v_data = (unsigned char *)malloc(out_W*input_H / 2);

    printf("%s %s %d %d %d\n",inputfilename, outputfilename, input_W, input_H, out_W);
    if(input_W <= out_W) {
        color_printf(_ERROR_, "cut tail: input file or output width value error!\n");
        goto out;
    }

    if (!inputfile) {
        color_printf(_ERROR_, "cut tail: Unable to open file for reading.\n");
        goto out;
    }

    if (!output_file) {
        color_printf(_ERROR_, "cut tail: Unable to open file for writing.\n");
        goto out;
    }

    fread(y_data, sizeof(uint8_t), input_W*input_H, inputfile);
    fread(u_data, sizeof(uint8_t), input_W*input_H/2 , inputfile);
    fread(v_data, sizeof(uint8_t), input_W*input_H/2, inputfile);

    // cut  tail
    for (int i = 0; i < input_H; i++) {
        memcpy(out_y_data+out_W*i, y_data+input_W*i, out_W);
        memcpy(out_u_data+out_W/2*i, u_data+input_W/2*i, out_W/2);
        memcpy(out_v_data+out_W/2*i, v_data+input_W/2*i, out_W/2);
    }

    fwrite(out_y_data, 1, out_W*input_H, output_file);  // 将数据写入新的文件
    fwrite(out_u_data, 1, out_W*input_H/2, output_file);  // 将数据写入新的文件
    fwrite(out_v_data, 1, out_W*input_H/2, output_file);  // 将数据写入新的文件

    // 刷新缓冲区确保数据被写入磁盘
    if (fflush(output_file) != 0) {
        color_printf(_ERROR_, "cut tail: fail to fflush output file\n");
    } 

    free(y_data);
    free(u_data);
    free(v_data);
    free(out_y_data);
    free(out_u_data);
    free(out_v_data);

out:
    fclose(inputfile);
    fclose(output_file);
}
