add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

add_executable(test_basic_greedy
    test_greedy.cpp)

target_link_libraries(test_basic_greedy sfo_cpp)