cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Fixed MSVC Runtime selection
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EOS_SDK_PATH "${CMAKE_SOURCE_DIR}/EOS_SDK")


if(WIN32)
    message(STATUS "Configuring EOS SDK for Windows")
    set(EOS_DLL "${EOS_SDK_PATH}/Bin/EOSSDK-Win64-Shipping.dll")
    set(EOS_LIB "${EOS_SDK_PATH}/Lib/EOSSDK-Win64-Shipping.lib")
elseif(UNIX AND NOT APPLE)
   message(STATUS "Configuring EOS SDK for Linux")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(EOS_DLL "${EOS_SDK_PATH}/Bin/libEOSSDK-Linux-Shipping.so")
        set(EOS_LIB ${EOS_DLL})
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set(EOS_DLL "${EOS_SDK_PATH}/Bin/libEOSSDK-LinuxArm64-Shipping.so")
        set(EOS_LIB ${EOS_DLL})
    else()
        message(FATAL_ERROR "Unsupported Linux architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif()


if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    set(REQUIRED_PACKAGES x11 xrandr gl glu freetype2 openal sndfile)
    pkg_check_modules(SFML_DEPS REQUIRED IMPORTED_TARGET ${REQUIRED_PACKAGES})
    
    if(NOT SFML_DEPS_FOUND)
        message(FATAL_ERROR "SFML dependencies not found. Run: sudo apt-get install build-essential libx11-dev libxrandr-dev libgl1-mesa-dev libglu1-mesa-dev libfreetype6-dev libopenal-dev libsndfile1-dev")
    endif()
endif()


include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# --- Executable Setup ---
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(HEXAGONS-GAME ${SOURCES}) # Renamed 'main' to 'HEXAGONS-GAME' for professional packaging

if(MSVC)
    target_compile_options(HEXAGONS-GAME PRIVATE /wd4455)
else()
    target_compile_options(HEXAGONS-GAME PRIVATE -Wno-literal-suffix)
endif()

target_include_directories(HEXAGONS-GAME PRIVATE include "${EOS_SDK_PATH}/Include")
target_link_libraries(HEXAGONS-GAME PRIVATE 
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
    "${EOS_LIB}"
)

if(UNIX AND NOT APPLE)
    target_link_libraries(HEXAGONS-GAME PRIVATE PkgConfig::SFML_DEPS)
endif()

# --- Path Logic & Options ---
option(PRODUCTION_BUILD "Set to ON for release builds" OFF)
option(SYSTEM_INSTALL "Set to ON if installing to /usr/share (Debian packaging)" OFF)

if(SYSTEM_INSTALL)
    # Paths for Linux Installation (.deb)
    target_compile_definitions(HEXAGONS-GAME PUBLIC ASSET_PATH="/usr/share/HEXAGONS-GAME/assets/")
    target_compile_definitions(HEXAGONS-GAME PUBLIC MAP_PATH="/usr/share/HEXAGONS-GAME/maps/")
elseif(PRODUCTION_BUILD)
    # Paths for portable release (zip/exe)
    target_compile_definitions(HEXAGONS-GAME PUBLIC ASSET_PATH="./assets/")
    target_compile_definitions(HEXAGONS-GAME PUBLIC MAP_PATH="./maps/")
else()
    # Paths for development
    target_compile_definitions(HEXAGONS-GAME PUBLIC ASSET_PATH="${CMAKE_SOURCE_DIR}/assets/")
    target_compile_definitions(HEXAGONS-GAME PUBLIC MAP_PATH="${CMAKE_SOURCE_DIR}/maps/")
endif()

# --- Post-Build (Local Debugging/Portable) ---
# Copy EOS DLL to bin folder
add_custom_command(TARGET HEXAGONS-GAME POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EOS_DLL}" $<TARGET_FILE_DIR:HEXAGONS-GAME>
)

# Copy Assets/Maps locally if not doing a system install
if(NOT SYSTEM_INSTALL)
    add_custom_command(TARGET HEXAGONS-GAME POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:HEXAGONS-GAME>/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/maps $<TARGET_FILE_DIR:HEXAGONS-GAME>/maps
        COMMENT "Copying assets and maps to build directory"
    )
endif()

# --- Installation & Packaging ---

# 1. Install Binary
install(TARGETS HEXAGONS-GAME DESTINATION bin)

# 2. Install Assets & Maps to /usr/share/HEXAGONS-GAME/
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION share/HEXAGONS-GAME)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/maps DESTINATION share/HEXAGONS-GAME)

# 3. Install EOS Library to /usr/lib/
install(PROGRAMS "${EOS_DLL}" DESTINATION lib)

# 4. Set RPATH so the app finds the library in /usr/lib or ../lib
set_target_properties(HEXAGONS-GAME PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN"
)

# --- CPack Configuration ---
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "hexagons-game")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_CONTACT "hexagonteams@example.com")
set(CPACK_PACKAGE_DESCRIPTION "Hexagons Game with EOS Integration")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPENDS ON) 
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Hexagon Teams") 
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

include(CPack)
