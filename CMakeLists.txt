cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

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

file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")

set(GAME_LOGIC_SOURCES ${ALL_SOURCES})
list(FILTER GAME_LOGIC_SOURCES EXCLUDE REGEX "main\\.cpp$")

option(PRODUCTION_BUILD "Set to ON for release builds" OFF)

if(PRODUCTION_BUILD)
    set(DEF_ASSET_PATH "./assets/")
    set(DEF_MAP_PATH "./maps/")
else()
    set(DEF_ASSET_PATH "${CMAKE_SOURCE_DIR}/assets/")
    set(DEF_MAP_PATH "${CMAKE_SOURCE_DIR}/maps/")
endif()

add_executable(main ${ALL_SOURCES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(main PRIVATE /wd4455)
else()
    target_compile_options(main PRIVATE -Wno-literal-suffix)
endif()

target_include_directories(main PRIVATE include)
target_include_directories(main PRIVATE "${EOS_SDK_PATH}/Include")

target_compile_features(main PRIVATE cxx_std_20)

target_compile_definitions(main PUBLIC "ASSET_PATH=\"${DEF_ASSET_PATH}\"")
target_compile_definitions(main PUBLIC "MAP_PATH=\"${DEF_MAP_PATH}\"")

target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)
target_link_libraries(main PRIVATE "${EOS_LIB}")

if(UNIX AND NOT APPLE)
    target_link_libraries(main PRIVATE PkgConfig::SFML_DEPS)
endif()

add_custom_command(TARGET main POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${EOS_DLL}"
      $<TARGET_FILE_DIR:main>
)

set(ASSET_DIR ${CMAKE_SOURCE_DIR}/assets)
set(MAP_DIR ${CMAKE_SOURCE_DIR}/maps) 

if(PRODUCTION_BUILD)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_DIR} $<TARGET_FILE_DIR:main>/assets
        COMMENT "Copying assets for production build"
    )

    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${MAP_DIR} $<TARGET_FILE_DIR:main>/maps
        COMMENT "Copying maps for production build"
    )
endif()


file(GLOB TEST_FILES "tests/*.cpp")

foreach(test_file ${TEST_FILES})
    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file} ${GAME_LOGIC_SOURCES})

    target_include_directories(${test_name} PRIVATE include)
    target_include_directories(${test_name} PRIVATE "${EOS_SDK_PATH}/Include")     
    target_compile_features(${test_name} PRIVATE cxx_std_20)
    
    target_compile_definitions(${test_name} PUBLIC "ASSET_PATH=\"${DEF_ASSET_PATH}\"")
    target_compile_definitions(${test_name} PUBLIC "MAP_PATH=\"${DEF_MAP_PATH}\"")
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${test_name} PRIVATE /wd4455)
    else()
        target_compile_options(${test_name} PRIVATE -Wno-literal-suffix)
    endif()

    target_link_libraries(${test_name} PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)
    
    target_link_libraries(${test_name} PRIVATE "${EOS_LIB}")
    if(UNIX AND NOT APPLE)
        target_link_libraries(${test_name} PRIVATE PkgConfig::SFML_DEPS)
    endif()

    add_custom_command(TARGET ${test_name} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${EOS_DLL}"
          $<TARGET_FILE_DIR:${test_name}>
    )

    message(STATUS "Registered Test: ${test_name}")
endforeach()