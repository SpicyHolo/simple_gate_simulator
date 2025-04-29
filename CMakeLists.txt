cmake_minimum_required(VERSION 3.10)
project(gate_sim)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include files
include_directories(include)

# Add Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add the gate_logic library
add_library(gate_logic src/gate_logic.cpp)

# Main application
add_executable(gate_sim src/main.cpp)
target_link_libraries(gate_sim gate_logic)

# Test binary
add_executable(gate_sim_test src/test.cpp)
target_link_libraries(gate_sim_test gate_logic ${GTEST_LIBRARIES} pthread)


# Add files
file(COPY ${CMAKE_SOURCE_DIR}/src/netlist.csv DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/src/netlists DESTINATION ${CMAKE_BINARY_DIR}/netlists)

add_custom_target(copy_files
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/netlist.csv ${CMAKE_BINARY_DIR}/netlist.csv
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/netlists ${CMAKE_BINARY_DIR}/netlists
)

add_dependencies(gate_sim_test copy_files)
add_dependencies(gate_sim copy_files)

# Add tests
enable_testing()
add_test(NAME MyTest COMMAND gate_sim_test)
