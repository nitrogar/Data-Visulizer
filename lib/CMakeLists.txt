# lib/CMakeLists.txt

# Add library code
add_library(MyLibrary 
  src/compute.cpp
)

# Specify include directories
target_include_directories(MyLibrary 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
