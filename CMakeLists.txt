cmake_minimum_required(VERSION 3.5)

project(graphPartitioner)

file(GLOB FILES *.cpp)

include_directories(include/gmsh)
include_directories(include/metis)

if(APPLE)
	message("OS X build")
	link_directories(lib/osx)
	set(EXECUTABLE_OUTPUT_PATH ../bin/osx)
else(APPLE)
	message("NIC4 build")
	link_directories(lib/nic4)
	set(EXECUTABLE_OUTPUT_PATH ../bin/nic4)
endif(APPLE)


add_executable(graphPartitioner ${FILES})


target_link_libraries(graphPartitioner Gmsh lapack blas metis)
