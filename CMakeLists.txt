cmake_minimum_required(VERSION 3.0)
project(bit_semestralka)

set(CMAKE_BUILD_TYPE Release)

find_library(GM 
	NAMES gmp libgmp 
	HINTS "${CMAKE_SOURCE_DIR}/lib/" "/usr/lib/" "/usr/local/lib/"
)

find_library(GMX 
	NAMES gmpxx 
	HINTS "${CMAKE_SOURCE_DIR}/lib/" "/usr/lib/" "/usr/local/lib"
)

message(INFO " gmp_path: - ${GM}")
message(INFO " gmpxx_path: - ${GMX}")

add_compile_options(-l${GMX} -l${GM} -std=c++11 -g)

include_directories(include ${CMAKE_SOURCE_DIR}/lib/include/)

file(GLOB SOURCES "src/*.cpp")

add_executable(main ${SOURCES})

target_link_libraries(main ${GMX} ${GM})
