cmake_minimum_required(VERSION 3.10)
project(MyGTKApp C)

# Trouver GTK4 avec pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Définir les fichiers sources dans le dossier models/
set(SOURCES models/main.c
)

# Créer l'exécutable
add_executable(mydiscord ${SOURCES})

# Ajouter les flags nécessaires pour GTK4
target_include_directories(mydiscord PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(mydiscord PRIVATE ${GTK4_LIBRARIES})
