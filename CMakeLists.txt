cmake_minimum_required(VERSION 4.1)
project(FlecTest)
set(CMAKE_CXX_STANDARD 20)

if (LINUX)
add_subdirectory("/home/matt/Desktop/Libraries/raylib-master" "extern/raylib")
add_subdirectory("/home/matt/Desktop/Libraries/flecs-master" "extern/flecs")
elseif(win32) # Idk, I dont use win
else()
    add_subdirectory("/Users/matttuliszewski/Desktop/Libraries/raylib-master" "extern/raylib")
    add_subdirectory("/Users/matttuliszewski/Desktop/Libraries/flecs-master" "extern/flecs")
endif()

add_executable(FlecTest main.cpp
        BoxCollisionFunctions.cpp
        BoxCollisionFunctions.h
        ComponentDeclarations.h)
target_link_libraries(FlecTest PUBLIC
        raylib
        flecs
)