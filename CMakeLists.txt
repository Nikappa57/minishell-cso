cmake_minimum_required(VERSION 3.10)
project(minishell C)

# compile flags
add_compile_options(-Wall -Wextra -Werror -g)

# headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# all *.c in src/
# file(GLOB SRCS "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB SRCS "${CMAKE_SOURCE_DIR}/src/*.c" "${CMAKE_SOURCE_DIR}/src/*/*.c")
add_executable(minishell ${SRCS})

# Link readline
target_link_libraries(minishell PRIVATE readline)
