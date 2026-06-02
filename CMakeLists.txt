cmake_minimum_required(VERSION 3.14)
project(CppEnhancedHeaders LANGUAGES CXX)

# ---- header-only 接口库 ----
add_library(ceh INTERFACE)
add_library(ceh::ceh ALIAS ceh)

target_include_directories(ceh INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(ceh INTERFACE cxx_std_17)

# 仅当作为顶层项目构建时，才编译示例与测试
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    option(CEH_BUILD_TESTS    "Build CppEnhancedHeaders tests"    ON)
    option(CEH_BUILD_EXAMPLES "Build CppEnhancedHeaders examples" ON)

    if(CEH_BUILD_TESTS)
        enable_testing()
        add_executable(test_big_int tests/test_big_int.cpp)
        target_link_libraries(test_big_int PRIVATE ceh::ceh)
        add_test(NAME big_int COMMAND test_big_int)
    endif()

    if(CEH_BUILD_EXAMPLES)
        add_executable(big_int_example examples/big_int_example.cpp)
        target_link_libraries(big_int_example PRIVATE ceh::ceh)
    endif()
endif()
