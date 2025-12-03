cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
        message(STATUS "Detected x86_64 architecture")
        set(EOS_DLL "${EOS_SDK_PATH}/Bin/libEOSSDK-Linux-Shipping.so")
        set(EOS_LIB ${EOS_DLL})
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        message(STATUS "Detected ARM64 architecture")
        set(EOS_DLL "${EOS_SDK_PATH}/Bin/libEOSSDK-LinuxArm64-Shipping.so")
        set(EOS_LIB ${EOS_DLL})
    else()
        message(FATAL_ERROR "Unsupported Linux architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif()

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    
    # SFML Dependencies
    set(REQUIRED_PACKAGES
        x11 xrandr gl glu freetype2 openal sndfile
    )

    pkg_check_modules(SFML_DEPS REQUIRED IMPORTED_TARGET ${REQUIRED_PACKAGES})
    
    if(NOT SFML_DEPS_FOUND)
        message(FATAL_ERROR "SFML dependencies not found.")
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

# Note: Recursive glob is generally discouraged for team projects, but okay for solo.
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(HEXAGONS-GAME ${SOURCES})

if(MSVC)
    target_compile_options(HEXAGONS-GAME PRIVATE /wd4455)
else()
    target_compile_options(HEXAGONS-GAME PRIVATE -Wno-literal-suffix)
endif()

target_include_directories(HEXAGONS-GAME PRIVATE include)
target_include_directories(HEXAGONS-GAME PRIVATE "${EOS_SDK_PATH}/Include")

target_compile_features(HEXAGONS-GAME PRIVATE cxx_std_20)

target_link_libraries(HEXAGONS-GAME PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)
target_link_libraries(HEXAGONS-GAME PRIVATE "${EOS_LIB}")


if(UNIX AND NOT APPLE)
    target_link_libraries(HEXAGONS-GAME PRIVATE PkgConfig::SFML_DEPS)
endif()

option(PRODUCTION_BUILD "Set to ON for release builds" OFF)
option(SYSTEM_INSTALL "Set to ON if installing to /usr/share (Debian packaging)" OFF)


if(SYSTEM_INSTALL)
    target_compile_definitions(HEXAGONS-GAME PUBLIC ASSET_PATH="/usr/share/HEXAGON-GAME/assets/")
elseif(PRODUCTION_BUILD)
    target_compile_definitions(HEXAGONS-GAME PUBLIC ASSET_PATH="./assets/")
else()
    target_compile_definitions(HEXAGONS-GAME PUBLIC ASSET_PATH="${CMAKE_SOURCE_DIR}/assets/")
endif()


add_custom_command(TARGET HEXAGONS-GAME POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${EOS_DLL}"
      $<TARGET_FILE_DIR:HEXAGONS-GAME>
)


if(NOT SYSTEM_INSTALL)
    add_custom_command(TARGET HEXAGONS-GAME POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:HEXAGONS-GAME>/assets
        COMMENT "Copying assets to binary directory"
    )
endif()

# === Packaging (.deb) ===
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "HEXAGONS-GAME")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_CONTACT "hexagonteams@example.com")
set(CPACK_PACKAGE_DESCRIPTION "Hexagons Game")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPENDS ON) 
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Hexagon Teams") 
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# Install binary
install(TARGETS HEXAGONS-GAME DESTINATION bin)

# Install assets
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION share/HEXAGON-GAME
        PATTERN ".git" EXCLUDE
        PATTERN "__pycache__" EXCLUDE)

install(PROGRAMS "${EOS_DLL}" DESTINATION lib)
set_target_properties(HEXAGONS-GAME PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN"
)

include(CPack)
