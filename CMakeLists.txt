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

        add_executable(test_fraction tests/test_fraction.cpp)
        target_link_libraries(test_fraction PRIVATE ceh::ceh)
        add_test(NAME fraction COMMAND test_fraction)

        add_executable(test_safe_int tests/test_safe_int.cpp)
        target_link_libraries(test_safe_int PRIVATE ceh::ceh)
        add_test(NAME safe_int COMMAND test_safe_int)

        add_executable(test_complex tests/test_complex.cpp)
        target_link_libraries(test_complex PRIVATE ceh::ceh)
        add_test(NAME complex COMMAND test_complex)

        add_executable(test_big_decimal tests/test_big_decimal.cpp)
        target_link_libraries(test_big_decimal PRIVATE ceh::ceh)
        add_test(NAME big_decimal COMMAND test_big_decimal)

        add_executable(test_binary_search_tree tests/test_binary_search_tree.cpp)
        target_link_libraries(test_binary_search_tree PRIVATE ceh::ceh)
        add_test(NAME binary_search_tree COMMAND test_binary_search_tree)

        add_executable(test_math tests/test_math.cpp)
        target_link_libraries(test_math PRIVATE ceh::ceh)
        add_test(NAME math COMMAND test_math)

        add_executable(test_sort tests/test_sort.cpp)
        target_link_libraries(test_sort PRIVATE ceh::ceh)
        add_test(NAME sort COMMAND test_sort)

        add_executable(test_search tests/test_search.cpp)
        target_link_libraries(test_search PRIVATE ceh::ceh)
        add_test(NAME search COMMAND test_search)
    endif()

    if(CEH_BUILD_EXAMPLES)
        add_executable(big_int_example examples/big_int_example.cpp)
        target_link_libraries(big_int_example PRIVATE ceh::ceh)

        add_executable(fraction_example examples/fraction_example.cpp)
        target_link_libraries(fraction_example PRIVATE ceh::ceh)

        add_executable(safe_int_example examples/safe_int_example.cpp)
        target_link_libraries(safe_int_example PRIVATE ceh::ceh)

        add_executable(complex_example examples/complex_example.cpp)
        target_link_libraries(complex_example PRIVATE ceh::ceh)

        add_executable(big_decimal_example examples/big_decimal_example.cpp)
        target_link_libraries(big_decimal_example PRIVATE ceh::ceh)

        add_executable(binary_search_tree_example examples/binary_search_tree_example.cpp)
        target_link_libraries(binary_search_tree_example PRIVATE ceh::ceh)

        add_executable(math_example examples/math_example.cpp)
        target_link_libraries(math_example PRIVATE ceh::ceh)

        add_executable(sort_example examples/sort_example.cpp)
        target_link_libraries(sort_example PRIVATE ceh::ceh)

        add_executable(search_example examples/search_example.cpp)
        target_link_libraries(search_example PRIVATE ceh::ceh)
    endif()
endif()
