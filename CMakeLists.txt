cmake_minimum_required(VERSION 3.5)

project(graphPartitioner)

file(GLOB FILES *.cpp)

include_directories(include/gmsh)
include_directories(include/metis)

if(APPLE)
	link_directories(lib/OSX)
	set(EXECUTABLE_OUTPUT_PATH ../bin/OSX)
else(APPLE)
	link_directories(lib/NIC4)
	set(EXECUTABLE_OUTPUT_PATH ../bin/NIC4)
endif(APPLE)


add_executable(graphPartitioner ${FILES})


target_link_libraries(graphPartitioner Gmsh blas metis)
