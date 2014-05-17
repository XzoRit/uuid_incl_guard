cmake_minimum_required(VERSION 2.8)

project(uuid_incl_guard)

include_directories("/opt/local/include")
link_directories("/opt/local/lib")

set (${PROJECT_NAME}_headers "")
set (${PROJECT_NAME}_sources "uuid_incl_guard.cpp" "utils.cpp")

set (TEST_PROJECT_NAME test_${PROJECT_NAME})
set (${TEST_PROJECT_NAME}_headers ${${PROJECT_NAME}_headers})
set (${TEST_PROJECT_NAME}_sources "test_uuid_incl_guard.cpp" "utils.cpp")

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_headers}
  ${${PROJECT_NAME}_sources})

add_executable(${TEST_PROJECT_NAME}
  ${${TEST_PROJECT_NAME}_headers}
  ${${TEST_PROJECT_NAME}_sources})

set (${PROJECT_NAME}_libs
	libboost_program_options-mt.a
	libboost_system-mt.a
	libboost_filesystem-mt.a)
set (${TEST_PROJECT_NAME}_libs ${${PROJECT_NAME}_libs})

target_link_libraries(${PROJECT_NAME}
	${${PROJECT_NAME}_libs})

target_link_libraries(${TEST_PROJECT_NAME}
	${${TEST_PROJECT_NAME}_libs})
