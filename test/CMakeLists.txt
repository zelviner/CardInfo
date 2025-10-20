# [1] 测试配置 -----------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test)
enable_testing()

# [2] 查找依赖 -----------------------------------------------------
find_package(GTest REQUIRED)

# [3] 添加测试目标 --------------------------------------------------
function(add_integration_test name)
    file(GLOB_RECURSE files ${name}/*.cpp)
    add_executable(${name}_test ${files})
    target_include_directories(${name}_test PUBLIC
        # 包含头文件目录
        ${PROJECT_SOURCE_DIR}/test
    )
    target_link_libraries(${name}_test
        PUBLIC
            GTest::gtest_main
            ${ARGN}
    )
endfunction(add_integration_test name)

# [4] 添加具体测试 --------------------------------------------------
add_integration_test(crypto zel)