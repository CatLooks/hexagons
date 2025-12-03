cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EOS_SDK_PATH "${CMAKE_SOURCE_DIR}/EOS_SDK")

if(WIN32)
    # --- Windows Settings ---
    message(STATUS "Configuring EOS SDK for Windows")
    set(EOS_DLL "${EOS_SDK_PATH}/Bin/EOSSDK-Win64-Shipping.dll")
    set(EOS_LIB "${EOS_SDK_PATH}/Lib/EOSSDK-Win64-Shipping.lib")
elseif(UNIX AND NOT APPLE)
    # --- Linux (and other UNIX-like OS) Settings ---
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

    set(REQUIRED_PACKAGES
        x11
        xrandr
        gl
        glu
        freetype2
        openal
        sndfile
    )

    pkg_check_modules(SFML_DEPS IMPORTED_TARGET ${REQUIRED_PACKAGES})

    if(NOT SFML_DEPS_FOUND)
        message(FATAL_ERROR "SFML dependencies not found. On Debian/Ubuntu, please run:\n"
                            "sudo apt-get install build-essential libx11-dev libxrandr-dev libgl1-mesa-dev "
                            "libglu1-mesa-dev libfreetype6-dev libopenal-dev libsndfile1-dev")
    endif()

    include_directories(${SFML_DEPS_INCLUDE_DIRS})
    link_directories(${SFML_DEPS_LIBRARY_DIRS})
endif()

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(main ${SOURCES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(main PRIVATE /wd4455)
else()
    target_compile_options(main PRIVATE -Wno-literal-suffix)
endif()

target_include_directories(main PRIVATE include)
target_include_directories(main PRIVATE "${EOS_SDK_PATH}/Include")

target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)
target_link_libraries(main PRIVATE "${EOS_LIB}")

option(PRODUCTION_BUILD "Set to ON for release builds" OFF)

if(PRODUCTION_BUILD)
    target_compile_definitions(main PUBLIC "ASSET_PATH=\"./assets\"")
else()
    target_compile_definitions(main PUBLIC "ASSET_PATH=\"${CMAKE_SOURCE_DIR}/assets/\"")
endif()

set(ASSET_DIR ${CMAKE_SOURCE_DIR}/assets)

add_custom_command(TARGET main POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${EOS_DLL}"
      $<TARGET_FILE_DIR:main>
)

if(PRODUCTION_BUILD)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_DIR} $<TARGET_FILE_DIR:main>/assets
        COMMENT "Copying assets for production build"
    )
endif()

# === Packaging (.deb) ===
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "HEXAGONS-GAME")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_CONTACT "hexagonteams@example.com")
set(CPACK_PACKAGE_DESCRIPTION "Hexagons Game")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPENDS OFF)
set(CPACK_DEBIAN_PACKAGE_HEXAGONS-GAMETAINER "Hexagon Teams")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# Install binary
install(TARGETS HEXAGONS-GAME DESTINATION bin)

# === Install assets ===
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION share/HEXAGON-GAME
        PATTERN ".git" EXCLUDE
        PATTERN "__pycache__" EXCLUDE)

# === Define asset path in code (for .deb) ===
target_compile_definitions(HEXAGONS-GAME PRIVATE ASSET_PATH="/usr/share/HEXAGON-GAME/assets/")

include(CPack)
