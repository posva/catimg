cmake_minimum_required(VERSION 2.8)
project(catimg)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if (NOT MAN_OUTPUT_PATH)
  set(MAN_OUTPUT_PATH "share/man/man1")
endif()

# set some options
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -g -std=c99 -Wno-unused-result")
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -Os -std=c99 -Wno-unused-result")
set(CMAKE_BUILD_TYPE Release)

set(SRC ${PROJECT_SOURCE_DIR}/src)

# Include dir for the exc
include_directories(${SRC})

set(EXECUTABLE_OUTPUT_PATH bin)

# find files
file(GLOB_RECURSE source_files ${SRC}/*)

# catimg executable
add_executable(
  catimg
  ${source_files}
  )

install(TARGETS catimg
  RUNTIME DESTINATION bin)

# man page
install(FILES ${PROJECT_SOURCE_DIR}/man/catimg.1
  DESTINATION ${MAN_OUTPUT_PATH})

# libs
target_link_libraries(catimg m)

# tests
#enable_testing()
#add_test(tcatimg ${PROJECT_SOURCE_DIR}/tests/tcatimg.sh)

# Doc
find_package(Doxygen)
if(DOXYGEN_FOUND)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)
