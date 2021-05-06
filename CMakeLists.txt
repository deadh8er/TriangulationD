# CMakeList.txt: проект CMake для TriangulationD; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project ("TriangulationD")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (TriangulationD "TriangulationD.cpp" "TriangulationD.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# Start Testing
enable_testing()

add_executable(
  testgtest
  testgtest.cpp
)
target_link_libraries(
  testgtest
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(testgtest)