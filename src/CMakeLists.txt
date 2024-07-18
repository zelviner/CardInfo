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
# add_executable(${APP_NAME}  ${SOURCES} ${RESOURCES}) #debug
add_executable(${APP_NAME} WIN32 ${SOURCES} ${RESOURCES}  ${MY_VERSIONINFO_RC}) # release

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

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
        "libcrypto32MD.lib"
        "libssl32MD.lib"
        "Crypt32.lib"   
        "ws2_32.lib"
        "zel.lib"
        "libmysql.lib"
)