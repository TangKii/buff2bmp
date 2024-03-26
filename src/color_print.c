#include "color_print.h"
#include <stdio.h>
#include <stdarg.h>

void color_printf(ErrorLevel level, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // 根据错误等级输出相应的颜色代码和标识
    switch (level) {
        case _PASS_:
            printf("%s[PASS] ", GREEN);
            break;
        case _WARNING_:
            printf("%s[WARNING] ", YELLOW);
            break;
        case _ERROR_:
            printf("%s[ERROR] ", RED);
            break;
    }

    // 重置文本颜色
    printf("%s", RESET);

    // 输出格式化文本
    vprintf(format, args);
    
    va_end(args);
}
