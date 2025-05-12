cmake_minimum_required(VERSION 3.10)

project(Mydiscord)

# Find GTK4 with pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Include the models directory for header files
include_directories(models)

# Set source files in folder models/
set(SOURCES 
    main.c
    models/home_page.c
)

# Create executable file (mydiscord.exe)
add_executable(mydiscord ${SOURCES})

# Add the necessary flags for GTK4
target_include_directories(mydiscord PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(mydiscord PRIVATE ${GTK4_LIBRARIES})

