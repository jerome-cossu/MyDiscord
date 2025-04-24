cmake_minimum_required(VERSION 3.10)

# Définir le nom du projet et la version
project(MyDiscord VERSION 1.0)

# Recherche de pkg-config et GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Définir le standard C++ utilisé (C++11 dans ce cas)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Ajouter les répertoires d'inclusion GTK4
include_directories(${GTK4_INCLUDE_DIRS})

# Ajouter les répertoires de bibliothèques GTK4
link_directories(${GTK4_LIBRARY_DIRS})

# Ajoute ton fichier source principal
add_executable(mydiscord main.c)

# Lier les bibliothèques GTK4 nécessaires à ton projet
target_link_libraries(mydiscord ${GTK4_LIBRARIES})
