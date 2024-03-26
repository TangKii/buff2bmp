# 设置最低的 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(decode)

# 设置输出目录为 bin
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# 添加可执行文件并指定源文件
add_executable(buff2bmp src/main.c src/split.c src/merge.c src/color_print.c)
#add_executable(split split.c)

# 添加包含目录
target_include_directories(buff2bmp PUBLIC include)

file(GLOB FILES_TO_COPY prebuild/*.exe)  # 使用通配符获取文件列表

foreach(FILE ${FILES_TO_COPY})  # 遍历文件列表
    file(COPY ${FILE} DESTINATION "${CMAKE_BINARY_DIR}/bin")  # 复制文件到目标目录
endforeach()