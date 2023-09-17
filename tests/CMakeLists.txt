add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

add_executable(test_monotone_greedy
    test_monotone_greedy.cpp)

add_executable(test_non_monotone_greedy
    test_non_monotone_greedy.cpp)

target_link_libraries(test_monotone_greedy sfo_cpp)
target_link_libraries(test_non_monotone_greedy sfo_cpp)