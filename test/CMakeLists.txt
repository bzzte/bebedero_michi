cmake_minimum_required(VERSION 3.16)

# Para encontrar el componente "bebedero" del proyecto principal
set(EXTRA_COMPONENT_DIRS "../components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_bebedero)