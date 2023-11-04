# 设置项目名称
project(MAIN)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定最低的CMake版本
cmake_minimum_required(VERSION 3.0)

# SET(CMAKE_CXX_COMPILER /opt/atk-dlrv1126-toolchain/usr/bin/arm-linux-gnueabihf-g++)

# 添加可执行文件的生成目标
add_executable(MAIN main.cpp DisplayEvent.cpp)

# 如果你的test.cpp文件依赖于test.h头文件，你可以使用target_include_directories命令
target_include_directories(MAIN PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MAIN -lpthread)

