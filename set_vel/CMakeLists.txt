cmake_minimum_required(VERSION 2.8.3)
project(set_vel)
find_package(catkin REQUIRED COMPONENTS
    roscpp
    mavros
    sensor_msgs
    geometry_msgs
    nav_msgs 
    roscpp 
    tf
)
catkin_package()
include_directories(
    ${catkin_INCLUDE_DIRS}
)
include_directories(include ${catkin_INCLUDE_DIRS})
add_executable(set_vel src/set_Vel.cpp)
target_link_libraries(set_vel ${catkin_LIBRARIES})
