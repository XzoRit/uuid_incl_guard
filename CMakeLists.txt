cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0021 OLD)

project(uuid_incl_guard)

include_directories("../boost")

set (${PROJECT_NAME}_headers "")
set (${PROJECT_NAME}_sources "uuid_incl_guard.cpp")

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_headers}
  ${${PROJECT_NAME}_sources})
