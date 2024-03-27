#include "global.h"
#include <unistd.h>
#include <windows.h>

uint32_t image_width = 0;  // 默认块宽度
uint32_t image_height = 0;
uint32_t encode_block_height = 0; // 默认块高度
uint32_t encode_block_size = 0;
uint32_t encode_block_cnt = 0;
long encode_file_size = 0;
uint32_t radio = 0, need_swap = 0;
int verbos_flag = 0;

unsigned char *y_data;
unsigned char *u_data;
unsigned char *v_data;

// 定义颜色代码
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define RESET   "\033[0m"


void execute_command(const char *format, ...) {
    char command[1024];  // 定义一个足够大的字符数组来保存命令字符串

    // 使用 va_list 和 va_start 宏来处理可变参数
    va_list args;
    va_start(args, format);

    // 使用 vsprintf 函数将可变参数格式化为命令字符串
    vsprintf(command, format, args);

    // 结束可变参数的处理
    va_end(args);

    // 调用 system 函数执行命令
    int ret = system(command);

    // 检查命令执行结果
    if (ret == 0) {
        color_printf(_PASS_, "\"%s\"\n",command);
    } else {
        color_printf(_ERROR_, "\"%s\"\n",command);
    }
}

int directory_exists(const char *path) {
    DWORD attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

void delete_directory_contents(const char *path) {
    WIN32_FIND_DATAA find_data;
    HANDLE hFind;
    char delete_path[MAX_PATH];

    snprintf(delete_path, sizeof(delete_path), "%s\\*.*", path);
    hFind = FindFirstFileA(delete_path, &find_data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
                snprintf(delete_path, sizeof(delete_path), "%s\\%s", path, find_data.cFileName);
                if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    delete_directory_contents(delete_path);
                    RemoveDirectoryA(delete_path);
                } else {
                    DeleteFileA(delete_path);
                }
            }
        } while (FindNextFileA(hFind, &find_data) != 0);
        FindClose(hFind);
    }
}

void create_directory(const char *path) {
    CreateDirectoryA(path, NULL);
}

void mk_temp_dir(const char *dirname)
{
    if (directory_exists(dirname)) {
        //printf("split: Directory already exists. Clearing its contents...\n");
        delete_directory_contents(dirname);
    } else {
        //printf("split: Directory does not exist. Creating it...\n");
        create_directory(dirname);
    }
}

void get_block_cnt(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    uint8_t buffer[24];

    fseek(file, encode_block_size-2, SEEK_SET);
    do {
        size_t bytes_read = fread(buffer, 1, 4, file);
        fseek(file, encode_block_size-4, SEEK_CUR);
        encode_block_cnt++;
    }
    while(buffer[0] == 0xff && buffer[1] == 0x11 && buffer[2]==0xff && buffer[3] == 0x10);

    if(image_height <= 20){
        encode_block_height = image_height;
        image_height = encode_block_height*encode_block_cnt;
    }

    encode_file_size = encode_block_cnt * encode_block_size;
    radio = image_width * encode_block_height * 2 /encode_block_size;

    if(verbos_flag) {
        printf("width %d, height %d, flie size %d, block size %d, block cnt %d\n", image_width, image_height, encode_file_size, encode_block_size, encode_block_cnt); 
    }
}

int get_jxs_head(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    uint8_t buffer[24];

    size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);  // 从源文件读取数据
    if(buffer[0] == 0xff && buffer[1] == 0x10 && buffer[6]==0xff && buffer[7] == 0x12) {
        encode_block_size = (buffer[10] << 24) | (buffer[11] << 16) | (buffer[12] << 8) | buffer[13];
        image_width = (buffer[18] << 8) | buffer[19];
        image_height = (buffer[20] << 8) | buffer[21];

    }
    else if(buffer[0] == 0x12 && buffer[1] == 0xFF && buffer[6]==0x10 && buffer[7] == 0xff) {

        encode_block_size = (buffer[13] << 24) | (buffer[12] << 16) | (buffer[11] << 8) | buffer[10];
        image_width = (buffer[21] << 8) | buffer[20];
        image_height = (buffer[19] << 8) | buffer[18];
        // LE need swap
        need_swap = 1;
    }
    else {
        color_printf(_ERROR_, "frame buff data error, could not get mark ff10\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    size_t block_size = 0, file_size = 0;// 默认块大小
    char filename[256] = {0}, filepath[256] = {0};
    const char *dirname = "..\\temp";

    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                verbos_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-v] <filename>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: Missing filename parameter.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(filepath,argv[optind]);
    strcpy(filename,strrchr(argv[optind], '\\') + 1);

    get_jxs_head(filepath);

    if(need_swap) {
        // LE BE swap
        if(verbos_flag)
            fprintf(stdout, "framebuff image data save as LE, need swap to BE\n");
        execute_command(".\\convertEndian.exe -n 8  %s %s_BE" ,filepath, filepath);
        strcat(filepath,"_BE");
    }
    else {
        fprintf(stdout, "no need swap to BE\n");
    }

    get_block_cnt(filepath);//LE必须先swap才能计算block cnt

    if(encode_block_height){
        if(verbos_flag)
            fprintf(stdout, "framebuff image data enode as %d row,need split and merge\n", encode_block_height);
        char split_output_filename[256];
        snprintf(split_output_filename, sizeof(split_output_filename), ".\\%s\\%s", dirname, filename);
        mk_temp_dir(dirname);
        split_file(filepath, split_output_filename);
        execute_command(".\\jxs_decoder.exe -f 0 -n %d %s.\\%s_%%d %s.\\%s_%%d.yuv",encode_block_cnt, dirname, filename, dirname, filename);
        char merge_output_filename[256];
        snprintf(merge_output_filename, sizeof(merge_output_filename), "%s.yuv", filepath);
        merge_file(split_output_filename, merge_output_filename);
        
    }
    else {
        if(verbos_flag)
            fprintf(stdout, "no need split and merge\n"); 
        execute_command(".\\jxs_decoder.exe %s %s.yuv",filepath, filepath);
    }

    execute_command(".\\yuv2bmp.exe -w %d -h %d %s.yuv %s.bmp", image_width, image_height, filepath, filepath);

    return 0;
}
