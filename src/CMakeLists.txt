# 设置应用程序名
set(APP_NAME CardInfo)

# 设置二进制文件输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 设置Qt5
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 寻找Qt5库
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# 设置UI文件搜索路径
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/res/ui)

# 查找源文件
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp ${CMAKE_CURRENT_LIST_DIR}/*.hpp)

# 添加资源文件
file(GLOB RESOURCES ${CMAKE_SOURCE_DIR}/res/rc/*)

# 添加可执行文件
add_executable(${APP_NAME}  ${SOURCES} ${RESOURCES}) #debug
# add_executable(${APP_NAME} WIN32 ${SOURCES} ${RESOURCES}  ${MY_VERSIONINFO_RC}) # release

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

# 添加编译时的宏定义
target_compile_definitions(${APP_NAME}
PUBLIC
    # 1. NOLFS: 自定义宏，用于禁用某些文件系统（LFS）相关的特性或功能
    NOLFS

    # 2. _CRT_SECURE_NO_WARNINGS: 禁用 Microsoft Visual Studio 对不安全 C 运行时函数的警告
    #    例如，禁用对使用 `strcpy`、`scanf` 等函数时的警告
    _CRT_SECURE_NO_WARNINGS

    # 3. _WINSOCK_DEPRECATED_NO_WARNINGS: 禁用关于过时的 Windows Sockets (Winsock) API 的警告
    #    用于忽略如 `recv`、`send` 等函数的过时警告
    _WINSOCK_DEPRECATED_NO_WARNINGS

    # 4. BUILDING_DLL: 用于标识正在编译一个动态链接库（DLL）
    #    在代码中可以使用 `#ifdef BUILDING_DLL` 来启用或禁用 DLL 特定的功能
    BUILDING_DLL
)

# # 为target添加头文件
# target_include_directories(${APP_NAME}
#     PUBLIC
# )

# # 为 target 添加库文件目录 (如果有需要，可以填入库文件目录路径)
# target_link_directories(${APP_NAME}
#     PUBLIC
#         path/to/libraries
# )

# 为target添加需要链接的共享库
TARGET_LINK_LIBRARIES(${APP_NAME}
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        VC/libcrypto32MD
        VC/libssl32MD
        Crypt32
        ws2_32
        zel
        libmysql
)
