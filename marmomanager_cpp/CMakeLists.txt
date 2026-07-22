cmake_minimum_required(VERSION 3.10)
project(MarmoManager VERSION 1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(MARMOMANAGER_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MARMOMANAGER_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MARMOMANAGER_BIN_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

find_package(Qt5 COMPONENTS Core Widgets Sql Test REQUIRED)

# Contratos de dados compartilhados entre as camadas. Não gera componente binário.
add_library(marmomanager_contracts INTERFACE)
target_include_directories(marmomanager_contracts INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/includes"
)
target_link_libraries(marmomanager_contracts INTERFACE Qt5::Core)

# Camada 1: acesso a dados relacionais e segurança de credenciais.
add_library(marmomanager_data SHARED
    src/database.cpp
    src/historico_repository.cpp
    src/os_repository.cpp
    src/password_hasher.cpp
    src/user_repository.cpp
    includes/database.h
    includes/historico_repository.h
    includes/os_repository.h
    includes/password_hasher.h
    includes/user_repository.h
)
target_link_libraries(marmomanager_data
    PUBLIC marmomanager_contracts Qt5::Sql
    PRIVATE Qt5::Core
)

# Camada 2: regras de negócio. Os repositórios aparecem apenas nos .cpp.
add_library(marmomanager_business SHARED
    src/application_controller.cpp
    src/auth_controller.cpp
    src/os_controller.cpp
    includes/application_controller.h
    includes/auth_controller.h
    includes/os_controller.h
)
target_link_libraries(marmomanager_business
    PUBLIC marmomanager_contracts Qt5::Core
    PRIVATE marmomanager_data
)

# Camada 3: apresentação Qt Widgets.
add_library(marmomanager_presentation SHARED
    src/ui/application_coordinator.cpp
    src/ui/history_dialog.cpp
    src/ui/login_dialog.cpp
    src/ui/main_window.cpp
    src/ui/application_coordinator.h
    src/ui/history_dialog.h
    src/ui/login_dialog.h
    src/ui/main_window.h
)
target_include_directories(marmomanager_presentation PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui"
)
target_link_libraries(marmomanager_presentation
    PUBLIC marmomanager_contracts Qt5::Widgets
    PRIVATE marmomanager_business
)

# Executável mínimo: conhece somente a fachada da camada de apresentação.
add_executable(marmomanager_app src/main.cpp)
target_link_libraries(marmomanager_app PRIVATE marmomanager_presentation)

set(MARMOMANAGER_COMPONENTS
    marmomanager_data
    marmomanager_business
    marmomanager_presentation
    marmomanager_app
)
set_target_properties(${MARMOMANAGER_COMPONENTS} PROPERTIES
    BUILD_RPATH "\$ORIGIN"
    INSTALL_RPATH "\$ORIGIN"
)

enable_testing()

add_executable(marmomanager_tests tests/unit/test_core.cpp)
target_link_libraries(marmomanager_tests PRIVATE
    marmomanager_business marmomanager_data Qt5::Test
)
add_test(NAME CoreTests COMMAND marmomanager_tests)

add_executable(marmomanager_gui_tests tests/func/test_func_gui.cpp)
target_link_libraries(marmomanager_gui_tests PRIVATE
    marmomanager_presentation marmomanager_business marmomanager_data Qt5::Test
)
add_test(NAME GuiTests COMMAND marmomanager_gui_tests)

add_executable(marmomanager_login_tests tests/func/test_login_gui.cpp)
target_link_libraries(marmomanager_login_tests PRIVATE
    marmomanager_presentation marmomanager_business marmomanager_data Qt5::Test
)
add_test(NAME LoginGuiTests COMMAND marmomanager_login_tests)

add_executable(marmomanager_acceptance_tests tests/acceptance/test_acceptance_bdd.cpp)
target_link_libraries(marmomanager_acceptance_tests PRIVATE
    marmomanager_presentation marmomanager_business marmomanager_data Qt5::Test
)
add_test(NAME AcceptanceBDDTests COMMAND marmomanager_acceptance_tests)

set_tests_properties(CoreTests GuiTests LoginGuiTests AcceptanceBDDTests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(documentation
        COMMAND "${DOXYGEN_EXECUTABLE}" Doxyfile
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs"
        COMMENT "Gerando documentação HTML em docs/html"
    )
endif()
