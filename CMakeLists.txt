cmake_minimum_required(VERSION 3.10)
project(MyGTKApp C)

# Find GTK4 with pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Set source files in folder models/
set(SOURCES models/main.c
)

# create executable file.exe
add_executable(mydiscord ${SOURCES})

# Add the necessary flags for GTK4
target_include_directories(mydiscord PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(mydiscord PRIVATE ${GTK4_LIBRARIES})
