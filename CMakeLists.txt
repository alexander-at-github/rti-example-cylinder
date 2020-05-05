cmake_minimum_required(VERSION 3.11)

project (
  use-librtidevice
  VERSION 0.1)

find_package (
  librtidevice REQUIRED
  # Path to the rti library
  PATHS "/home/alexander/iue/cdr/rti-3-tmp/build/"
  )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# hack: gnuplot-iostream uses boost iostreams
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lboost_iostreams")

add_executable (
  run "src/main.cpp"
  )
target_link_libraries (
  run
  PRIVATE
  librtidevice::librtidevice
  )
