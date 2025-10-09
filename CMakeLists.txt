cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

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

target_include_directories(main PRIVATE include)

target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)


option(PRODUCTION_BUILD "Set to ON for release builds" OFF)

if(PRODUCTION_BUILD)
    target_compile_definitions(main PUBLIC "RESOURCES_PATH=\"./resources/\"")
else()
    target_compile_definitions(main PUBLIC "RESOURCES_PATH=\"${CMAKE_SOURCE_DIR}/resources/\"")
endif()

set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resources)


if(PRODUCTION_BUILD)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCE_DIR} $<TARGET_FILE_DIR:main>/resources
        COMMENT "Copying resources for production build"
    )
endif()
