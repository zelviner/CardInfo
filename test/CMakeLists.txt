# 设置测试程序的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test)

# 查找 GTest 库
find_package(GTest REQUIRED)

# 启用测试
enable_testing()

# 定义添加测试执行文件的函数
function(add_test_executable name)
    file(GLOB_RECURSE files ${name}/*.cpp)
    add_executable(${name}-test ${files})
    target_include_directories(${name}-test 
        PUBLIC
           
    )
    target_link_libraries(${name}-test
        PUBLIC
            GTest::gtest_main
            ${ARGN}
    )
endfunction(add_test_executable name)

# 添加测试
add_test_executable(crpto zel)
