# [1] 项目设置 ----------------------------------------------------
cmake_minimum_required(VERSION 3.14)
project(CardInfo VERSION 0.1.0 LANGUAGES CXX)

# [2] vcpkg工具链配置 ----------------------------------------------
if(DEFINED ENV{ZEL_HOME})
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(VCPKG_TARGET_TRIPLET "x64-windows")
  else()
    set(VCPKG_TARGET_TRIPLET "x86-windows")
  endif()

  # 设置工具链文件
  set(CMAKE_TOOLCHAIN_FILE "$ENV{ZEL_HOME}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file" FORCE)
  
  # 自动搜索vcpkg包路径
  list(APPEND CMAKE_PREFIX_PATH "$ENV{ZEL_HOME}/installed/${VCPKG_TARGET_TRIPLET}")
  message(STATUS "[1] VCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}")
endif()

# [3] 全局安装路径配置 ----------------------------------------------
include(GNUInstallDirs)
if(DEFINED ENV{ZEL_HOME})
  set(CMAKE_INSTALL_PREFIX "$ENV{ZEL_HOME}/installed/${VCPKG_TARGET_TRIPLET}"
    CACHE PATH "Install path" FORCE)
else()
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/installed")
endif()
message(STATUS "[2] Install prefix: ${CMAKE_INSTALL_PREFIX}")

# [4] 全局编译选项 --------------------------------------------------
if(MSVC)
  add_compile_options(/utf-8 /W4 /WX)
else()
  add_compile_options(-Wall -Wextra)
endif()

# [5] 链接库配置路径 -------------------------------------------------
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  link_directories(${CMAKE_INSTALL_PREFIX}/debug/lib)
else()
  link_directories(${CMAKE_INSTALL_PREFIX}/lib)
endif()

# [6] 子目录添加 ----------------------------------------------------
add_subdirectory(src)
# add_subdirectory(test)