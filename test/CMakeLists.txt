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

# 查找 GTest 库
find_package(GTest REQUIRED)

# 启用测试
enable_testing()

# 添加测试
