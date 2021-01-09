add_ev3_executable(robot_arm           robot_arm.cpp)
add_executable(cube-solver cube_main.cpp cube_algorithm.cpp)
target_link_libraries(cube-solver ev3dev pthread)
