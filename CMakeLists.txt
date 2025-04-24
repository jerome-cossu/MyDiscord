cmake_minimum_required(VERSION 3.10)
project(Mydiscord)

# Find GTK4 with pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Manually specify PostgreSQL paths if find_package fails
# Adjust the paths to where PostgreSQL is installed on your system
set(PostgreSQL_INCLUDE_DIR "C:/msys64/ucrt64/include")
set(PostgreSQL_LIBRARY "C:/msys64/ucrt64/lib/libpq.dll.a")

# Include directories
include_directories(
    ${GTK4_INCLUDE_DIRS}
    ${PostgreSQL_INCLUDE_DIR}
    models
)

# Set source files
set(SOURCES 
    main.c
    models/home_page.c
)

# Create executable
add_executable(mydiscord ${SOURCES})

# Link libraries
target_link_libraries(mydiscord
    ${GTK4_LIBRARIES}
    ${PostgreSQL_LIBRARY}
    regex 
)
