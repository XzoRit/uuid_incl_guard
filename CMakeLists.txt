cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0021 OLD)

project(uuid_incl_guard)

include_directories("/opt/local/include")
link_directories("/opt/local/lib")

add_compile_options(-g)

set (${PROJECT_NAME}_headers "")
set (${PROJECT_NAME}_sources "uuid_incl_guard.cpp")

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_headers}
  ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME}
	libboost_program_options-mt.a
	libboost_system-mt.a
	libboost_filesystem-mt.a)
