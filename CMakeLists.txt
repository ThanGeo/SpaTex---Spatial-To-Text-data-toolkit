cmake_minimum_required(VERSION 3.0.0)
project(Main VERSION 1.0.0)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)  # Optional, ensures the use of -std=c++17 instead of -std=gnu++17

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(MSVC)
    add_compile_options(/std::c++latest)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif()

# supress warnings
add_definitions(-w)

set(SrcFiles
    src/def.cpp
    src/containers.cpp
    src/utils.cpp
    src/parse.cpp
    src/config.cpp

    src/index/create.cpp
    src/index/filter.cpp
    src/index/refinement.cpp
    
)

# supress warnings
add_definitions(-w)
include_directories(include)
add_library(${PROJECT_NAME} ${ProjectType} ${SrcFiles})

# find boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 COMPONENTS REQUIRED)

#main
add_executable(main main.cpp)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# link all
target_link_libraries(main ${PROJECT_NAME})
target_link_libraries(main ${Boost_LIBRARIES})