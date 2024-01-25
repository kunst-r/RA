cmake_minimum_required(VERSION 3.0)
project(raLab)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(raLab/dependencies/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_EXPORTER TRUE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE)
add_subdirectory(raLab/dependencies/assimp)


add_subdirectory(raLab/primjerGLM)
add_subdirectory(raLab/primjerASSIMP)
add_subdirectory(raLab/primjerOpenGL)


add_subdirectory(raLab/vjezba1)
add_subdirectory(raLab/vjezba2)
add_subdirectory(raLab/vjezba3)