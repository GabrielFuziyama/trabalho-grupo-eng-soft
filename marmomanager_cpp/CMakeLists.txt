cmake_minimum_required(VERSION 3.10)
project(MarmoManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Find Qt5 modules
find_package(Qt5 COMPONENTS Core Widgets Sql Test REQUIRED)

# Output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Include directories
include_directories(includes src/ui)

# Gather source and header files
file(GLOB SRC_FILES "src/*.cpp")
file(GLOB UI_FILES "src/ui/*.cpp" "src/ui/*.h")
file(GLOB HEADERS "includes/*.h")

# App Executable
add_executable(marmomanager_app ${SRC_FILES} ${UI_FILES} ${HEADERS})
target_link_libraries(marmomanager_app PRIVATE Qt5::Core Qt5::Widgets Qt5::Sql)

# Tests
enable_testing()
# Remove main.cpp from SRC_FILES to avoid "multiple definition of main"
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_executable(marmomanager_tests tests/unit/test_core.cpp ${SRC_FILES} ${UI_FILES} ${HEADERS})
target_link_libraries(marmomanager_tests PRIVATE Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Test)
add_test(NAME CoreTests COMMAND marmomanager_tests)

add_executable(marmomanager_gui_tests tests/func/test_func_gui.cpp ${SRC_FILES} ${UI_FILES} ${HEADERS})
target_link_libraries(marmomanager_gui_tests PRIVATE Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Test)
add_test(NAME GuiTests COMMAND marmomanager_gui_tests)
