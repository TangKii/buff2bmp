#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "color_print.h"

extern uint32_t image_width, image_height; // 默认块宽度 默认块高度
extern uint32_t encode_block_cnt, encode_block_size;
extern int verbos_flag;
extern uint32_t radio;
void split_file(char *inputfilename, const char *outputfilename);
void merge_file(char *inputfilename, const char *outputfilename);

#endif