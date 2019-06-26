cmake_minimum_required(VERSION 2.8.3)
project(mission_planner)
find_package(catkin REQUIRED COMPONENTS
    roscpp
    mavros
    sensor_msgs
    geometry_msgs
)
catkin_package()
include_directories(
    ${catkin_INCLUDE_DIRS}
)
include_directories(include ${catkin_INCLUDE_DIRS})
add_executable(mission_planner src/mission_planner.cpp)
target_link_libraries(mission_planner ${catkin_LIBRARIES})
