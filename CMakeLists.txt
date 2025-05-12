cmake_minimum_required(VERSION 3.10)
project(Mydiscord C)

# Find GTK4 using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Set source files
set(SOURCES 
    main.c
    models/home_page.c
    models/chat_page.c
)

# Create the executable
add_executable(mydiscord ${SOURCES})

# Add include directories
target_include_directories(mydiscord PRIVATE 
    ${GTK4_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)

# Add GTK4 compiler flags
target_compile_options(mydiscord PRIVATE ${GTK4_CFLAGS_OTHER})

# Link GTK4 and Windows socket library
target_link_libraries(mydiscord PRIVATE ${GTK4_LIBRARIES} ws2_32)
