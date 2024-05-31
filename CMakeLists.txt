cmake_minimum_required(VERSION 3.10)
project(Connect4)

set(CMAKE_CXX_STANDARD 11)

# Set paths for SDL2 and SDL2_ttf (Homebrew)
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH /opt/homebrew)
    include_directories(/opt/homebrew/include/SDL2)
    link_directories(/opt/homebrew/lib)
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Include directories for SDL2 and SDL2_ttf
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

# Add the executable
add_executable(Connect4 main.cpp)

# Link SDL2 and SDL2_ttf libraries
target_link_libraries(Connect4 ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
target_link_libraries(Connect4 SDL2_ttf ${SDL2_LIBRARIES})
