cmake_minimum_required(VERSION 2.8.3)
project(cwru_davinci_grasp)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  trajectory_msgs
  geometry_msgs
  moveit_msgs
  sensor_msgs
  convenience_ros_functions
  eigen_conversions
  cwru_davinci_moveit_object_handling
  moveit_ros_planning
  moveit_ros_planning_interface
  moveit_core
  moveit_visual_tools
  actionlib_msgs
  uv_control
  uv_msgs
  gazebo_state_plugins
  )

find_package(gazebo REQUIRED)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)

# C++11 required on kinetic for MoveIt
add_definitions(${baselib_binding_DEFINITIONS} ${convenience_ros_functions_DEFINITIONS} -std=c++11)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a run_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a run_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
#add_message_files(DIRECTORY msg
#  FILES
#  GraspData.msg
#  )


## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
add_action_files(
  DIRECTORY action
  FILES NeedleGrasp.action
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  actionlib_msgs std_msgs geometry_msgs
)

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a run_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES cwru_davinci_grasp
  CATKIN_DEPENDS roscpp std_msgs actionlib_msgs sensor_msgs geometry_msgs moveit_msgs trajectory_msgs moveit_visual_tools
                cwru_davinci_moveit_object_handling uv_msgs gazebo_state_plugins
  # DEPENDS
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${GAZEBO_INCLUDE_DIRS}
)

## Declare a C++ library
add_library(cwru_davinci_grasp
  src/davinci_needle_grasper_base.cpp
  src/davinci_simple_needle_grasper.cpp
  src/davinci_needle_grasp_data.cpp
  src/davinci_simple_grasp_generator.cpp
  src/davinci_needle_grasper_action_server.cpp)
#add_library(davinci_needle_grasper_action_server src/davinci_needle_grasper_action_server.cpp)

add_library(davinci_needle_pose_publisher src/davinci_needle_pose_publisher)

# add_dependencies(davinci_simple_grasp_action_server ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
#add_dependencies(davinci_simple_grasp_generator ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
add_executable(davinci_simple_needle_grasp_test_node test/davinci_simple_needle_grasp_test_node.cpp)
add_executable(davinci_simple_needle_grasper_main src/davinci_simple_needle_grasper_main.cpp)
add_executable(davinci_needle_pose_publisher_main test/davinci_needle_pose_publisher_main.cpp)
add_executable(davinci_test_grasping_main src/davinci_test_grasping_main.cpp)
add_executable(davinci_needle_grasper_action_server_main src/davinci_needle_grasper_action_server_main.cpp)
add_executable(davinci_grasp_info_writer src/davinci_grasp_info_writer.cpp)
add_executable(davinci_needle_pose_perturber test/davinci_needle_pose_perturber.cpp)
add_executable(needle_perturbation_statistics test/needle_perturbation_statistics.cpp)


## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
add_dependencies(cwru_davinci_grasp ${PROJECT_NAME}_generate_messages_cpp)

## Specify libraries to link a library or executable target against
#target_link_libraries(davinci_simple_grasp_generator ${catkin_LIBRARIES})

#target_link_libraries(davinci_simple_grasp_action_server ${catkin_LIBRARIES})
target_link_libraries(cwru_davinci_grasp ${catkin_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(davinci_simple_needle_grasp_test_node cwru_davinci_grasp  ${catkin_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(davinci_simple_needle_grasper_main cwru_davinci_grasp  ${catkin_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(davinci_needle_pose_publisher_main davinci_needle_pose_publisher ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES})
target_link_libraries(davinci_test_grasping_main cwru_davinci_grasp  ${catkin_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(davinci_needle_grasper_action_server_main cwru_davinci_grasp ${catkin_LIBRARIES})
target_link_libraries(davinci_grasp_info_writer cwru_davinci_grasp ${catkin_LIBRARIES})
target_link_libraries(davinci_needle_pose_perturber davinci_needle_pose_publisher ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES})
target_link_libraries(needle_perturbation_statistics davinci_needle_pose_publisher ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES})

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
 install(TARGETS ${PROJECT_NAME}
   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
 )

## Mark cpp header files for installation
 install(DIRECTORY include/${PROJECT_NAME}/
   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
   FILES_MATCHING PATTERN "*.h"
   PATTERN ".svn" EXCLUDE
 )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_cwru_davinci_grasp.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
