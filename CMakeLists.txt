# 最低版本
cmake_minimum_required(VERSION 3.14) 

# 设置项目名称
project(CardInfo)

# 采用C++14标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置安装路径
set(ZELPATH $ENV{ZELPATH}/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${ZELPATH})

if(WIN32)
    set(WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
    # Specify MSVC UTF-8 encoding   
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")    
endif()

# 设置三方库的安装路径, 搜索路径, 链接路径
list(APPEND CMAKE_PREFIX_PATH ${ZELPATH})
include_directories(${ZELPATH}/include)
link_directories(${ZELPATH}/lib)

# 添加子工程
add_subdirectory(src)
add_subdirectory(test)
