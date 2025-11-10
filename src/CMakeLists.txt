# [1] 设置应用程序名称 ------------------------------------------------
set(APP_NAME CardInfo)
string(TOUPPER ${APP_NAME} UPPER_APP_NAME)

# [2] 输出目录配置 -----------------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# [3] Qt 编译设置 -----------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# [4] 查找三方库 ------------------------------------------------------
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(zel COMPONENTS myorm REQUIRED)
find_package(unofficial-libmariadb CONFIG REQUIRED)

# [5] UI 搜索路径 -----------------------------------------------------
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/res/ui)

# [6] 查找源文件/资源 --------------------------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
)

file(GLOB RESOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/res/rc/*)

# [7] 添加可执行文件 ---------------------------------------------------
# add_executable(${APP_NAME} ${SOURCES} ${RESOURCES}) #debug
add_executable(${APP_NAME} WIN32 ${SOURCES} ${RESOURCES}  ${MY_VERSIONINFO_RC}) # release

# [8] 包含 Qt 的私有头（如果有需要，比如自定义窗口样式）--------------
include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

# [9] 添加宏定义 -------------------------------------------------------
target_compile_definitions(${APP_NAME} PUBLIC
)

# [10] 链接 Qt 库 ------------------------------------------------------
target_link_libraries(${APP_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    zel::myorm
    unofficial::libmariadb
)