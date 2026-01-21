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

if(UNIX AND NOT APPLE)
    target_link_libraries(main PRIVATE PkgConfig::SFML_DEPS)
endif()

option(PRODUCTION_BUILD "Set to ON for release builds" OFF)

if(PRODUCTION_BUILD)
    target_compile_definitions(main PUBLIC "ASSET_PATH=\"./assets\"")
    target_compile_definitions(main PUBLIC "MAP_PATH=\"./maps/\"")    
else()
    target_compile_definitions(main PUBLIC "ASSET_PATH=\"${CMAKE_SOURCE_DIR}/assets/\"")
    target_compile_definitions(main PUBLIC "MAP_PATH=\"${CMAKE_SOURCE_DIR}/maps/\"")
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

enable_testing()

# Unit tests

add_executable(random_tests tests/random_tests.cpp)
target_include_directories(random_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(random_tests PRIVATE cxx_std_20)
target_sources(random_tests PRIVATE
    src/random.cpp
)
add_test(NAME random_tests COMMAND random_tests)

target_link_libraries(random_tests PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)

add_executable(mathext_tests tests/mathext_tests.cpp)
target_include_directories(mathext_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(mathext_tests PRIVATE cxx_std_20)
target_sources(mathext_tests PRIVATE
    src/mathext.cpp
    src/localization/token.cpp
    src/localization/error.cpp
    src/localization/parser.cpp
    src/assetload.cpp
    src/assets.cpp
)

target_compile_definitions(mathext_tests PRIVATE
    "ASSET_PATH=\"${CMAKE_SOURCE_DIR}/assets/\""
    "MAP_PATH=\"${CMAKE_SOURCE_DIR}/maps/\""
)

add_test(NAME mathext_tests COMMAND mathext_tests)

target_link_libraries(mathext_tests PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)

add_executable(ui_units_tests tests/ui_units_tests.cpp)
target_include_directories(ui_units_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(ui_units_tests PRIVATE cxx_std_20)
target_sources(ui_units_tests PRIVATE
    src/ui/units.cpp
)
add_test(NAME ui_units_tests COMMAND ui_units_tests)

target_link_libraries(ui_units_tests PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)

add_executable(localization_tests tests/localization_tests.cpp)
target_include_directories(localization_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(localization_tests PRIVATE cxx_std_20)
target_sources(localization_tests PRIVATE
    src/localization/token.cpp
)
add_test(NAME localization_tests COMMAND localization_tests)

add_executable(hexarray_tests tests/hexarray_tests.cpp)
target_include_directories(hexarray_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(hexarray_tests PRIVATE cxx_std_20)
target_sources(hexarray_tests PRIVATE src/game/array.cpp src/game/hex.cpp) # jeœli hex.cpp istnieje
target_link_libraries(hexarray_tests PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)
add_test(NAME hexarray_tests COMMAND hexarray_tests)

# Component tests

add_executable(parser_integration_tests tests/parser_integration_tests.cpp)
target_include_directories(parser_integration_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(parser_integration_tests PRIVATE cxx_std_20)
target_sources(parser_integration_tests PRIVATE
    src/localization/token.cpp
    src/localization/error.cpp
    src/localization/parser.cpp
)
add_test(NAME parser_integration_tests COMMAND parser_integration_tests)

add_executable(spread_component_tests tests/spread_component_tests.cpp)
target_include_directories(spread_component_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(spread_component_tests PRIVATE cxx_std_20)
target_sources(spread_component_tests PRIVATE
    src/game/array.cpp
    src/game/hex.cpp
    src/game/spread.cpp
)
target_link_libraries(spread_component_tests PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)
add_test(NAME spread_component_tests COMMAND spread_component_tests)

add_executable(messages_light_tests tests/messages_light_tests.cpp)
target_include_directories(messages_light_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(messages_light_tests PRIVATE cxx_std_20)
target_link_libraries(messages_light_tests PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)
add_test(NAME messages_light_tests COMMAND messages_light_tests)

#Performance tests

add_executable(perf_random tests/perf_random.cpp)
target_include_directories(perf_random PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(perf_random PRIVATE cxx_std_20)
target_sources(perf_random PRIVATE
    src/random.cpp
)
target_link_libraries(perf_random PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)
add_test(NAME perf_random COMMAND perf_random)

add_executable(perf_dim tests/perf_dim.cpp)
target_include_directories(perf_dim PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(perf_dim PRIVATE cxx_std_20)
target_sources(perf_dim PRIVATE
    src/ui/units.cpp
)
target_link_libraries(perf_dim PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)
add_test(NAME perf_dim COMMAND perf_dim)

#Fuzz tests

add_executable(hexarray_fuzz tests/hexarray_fuzz.cpp)
target_include_directories(hexarray_fuzz PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(hexarray_fuzz PRIVATE cxx_std_20)
target_sources(hexarray_fuzz PRIVATE
    src/game/array.cpp
    src/game/hex.cpp
    src/random.cpp
)
target_link_libraries(hexarray_fuzz PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)
add_test(NAME hexarray_fuzz COMMAND hexarray_fuzz)

add_executable(mathext_fuzz tests/mathext_fuzz.cpp)
target_include_directories(mathext_fuzz PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(mathext_fuzz PRIVATE cxx_std_20)
target_sources(mathext_fuzz PRIVATE
    src/mathext.cpp
    src/random.cpp
    src/localization/token.cpp
    tests/mathext_fuzz_stubs.cpp
)
target_link_libraries(mathext_fuzz PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network
)
add_test(NAME mathext_fuzz COMMAND mathext_fuzz)