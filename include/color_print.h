#ifndef COLOR_PRINT_H
#define COLOR_PRINT_H

// 定义颜色代码
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define RED     "\033[0;31m"
#define RESET   "\033[0m"

// 定义错误等级
typedef enum {
    _PASS_,
    _WARNING_,
    _ERROR_
} ErrorLevel;

// 输出彩色文本的函数
void color_printf(ErrorLevel level, const char *format, ...);

#endif /* COLOR_PRINT_H */
