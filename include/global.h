#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "color_print.h"

extern uint32_t image_width, image_height; // 默认块宽度 默认块高度
extern uint32_t encode_block_cnt, encode_block_size, encode_block_width, encode_block_height;
extern long encode_file_size;
extern int verbos_flag;
extern float radio;
void split_file(char *inputfilename, const char *outputfilename);
void merge_file(char *inputfilename, const char *outputfilename);
void cut_w_tail(char *inputfilename, uint32_t input_W, uint32_t input_H, const char *outputfilename,  uint32_t out_W);
#endif