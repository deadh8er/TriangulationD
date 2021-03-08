# CMakeList.txt: проект CMake для TriangulationD; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project ("TriangulationD")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (TriangulationD "TriangulationD.cpp" "TriangulationD.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
