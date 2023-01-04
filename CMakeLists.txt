cmake_minimum_required(VERSION 2.8) # 要求cmake的最小版本
project(sylar)                      # 定义一个项目

set(CMAKE_VERBOSE_MAKEFILE ON)      # 要求在make过程中显示一些详细命令
# 自定义的一些编译参数放进去
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -o0 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")



# 将源码设置到LIB_SRC上
set(LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    )

add_library(sylar SHARED ${LIB_SRC})    # 添加 SHARED 库，生成 so 文件
# add_library(sylar_static STATIC ${LIB_SRC})
# SET_TARGET_PROPERTIES {sylar_static PROPERTIES OUTPUT_NAME "sylar"}

add_executable(test tests/test.cc)  # 生成一个测试文件test，使用的源文件是tests/test.cc
add_dependencies(test sylar)        # 依赖
target_link_libraries(test sylar)   # 链接于lib

add_executable(test_config tests/test_config.cc)  # 生成一个测试文件test，使用的源文件是tests/test.cc
add_dependencies(test_config sylar)        # 依赖
target_link_libraries(test_config sylar)   # 链接于lib

# 输出 lib 生成的路径
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

