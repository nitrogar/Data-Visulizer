# CMakeLists.txt

cmake_minimum_required(VERSION 3.28)

project(MyCppProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add all cpp files in the src directory to the executable
file(GLOB SOURCES "src/*.cpp")
file(GLOB SOURCES_COMPUTE_UNITS "src/compute_units/*.cpp")
list(APPEND SOURCES ${SOURCES_COMPUTE_UNITS})
## imgui bundle integration
add_subdirectory(/home/archiver/storage/githubs/imgui_bundle bin)


imgui_bundle_add_app(MyCppProject ${SOURCES})

#add_executable(MyCppProject ${SOURCES})

# Add library code
add_subdirectory(lib)  # Include the lib directory
# Specify include directories
target_include_directories(MyCppProject 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link the executable with the library
target_link_libraries(MyCppProject PRIVATE MyLibrary)

# Specify the output directory for the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)


# run after build
add_custom_target(run
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/MyCppProject
  DEPENDS MyCppProject
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

## H5 Integration

if(NOT DEFINED HIGHFIVE_USE_BOOST)
  set(HIGHFIVE_USE_BOOST Off)
endif()

# Include the subdirectory and use the target HighFive.
add_subdirectory(/home/archiver/storage/githubs/HighFive bin/h5)
target_link_libraries(MyCppProject PUBLIC HighFive)

target_link_libraries(MyCppProject PUBLIC OpenCL)
