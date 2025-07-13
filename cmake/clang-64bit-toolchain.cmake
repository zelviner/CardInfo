# clang-64bit-toolchain.cmake

# ---------------------- 平台 & 架构 ----------------------
set(CMAKE_SYSTEM_NAME Windows)

# 强制使用 64 位目标架构
set(CMAKE_C_FLAGS_INIT "-m64")
set(CMAKE_CXX_FLAGS_INIT "-m64")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-m64")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-m64")