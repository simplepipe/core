cmake_minimum_required(VERSION 2.8.9)
project (physics C)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_COMPILER "emcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_ZLIB=1")
    set(LIBRARY_INPUT "${CMAKE_INSTALL_PREFIX}")
else()
        if (${CMAKE_GENERATOR} MATCHES Xcode)
                set(LIBRARY_INPUT[variant=Debug] "${IOS_LIBRARY_PREIX}/debug")
                set(LIBRARY_INPUT[variant=Release] "${IOS_LIBRARY_PREIX}/release")
                set(LIBRARY_INPUT[variant=MinSizeRel] "${IOS_LIBRARY_PREIX}/release")
                set(LIBRARY_INPUT[variant=RelWithDebInfo] "${IOS_LIBRARY_PREIX}/release")
        else()
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
                set(LIBRARY_INPUT "${CMAKE_INSTALL_PREFIX}")
        endif()
endif()


add_library(math STATIC IMPORTED)
set_target_properties(
        math
        PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_INPUT}/libmath.a"
)

add_library(kernel STATIC IMPORTED)
set_target_properties(
        kernel
        PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_INPUT}/libkernel.a"
)

add_library(zip STATIC IMPORTED)
set_target_properties(
        zip
        PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_INPUT}/libzip.a"
)

include_directories(include)
include_directories(../math/include)
include_directories(../kernel/include)
include_directories(../zip/include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(GLOB_RECURSE SOURCES "src/*.c" "src/*.m")
else()
    file(GLOB_RECURSE SOURCES "src/*.c")
endif()

add_library(physics STATIC ${SOURCES})
target_link_libraries(physics kernel math m zip z)

if(${CMAKE_BUILD_TYPE} MATCHES "Release" AND ${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_custom_command(
                TARGET physics
                POST_BUILD
                COMMAND ${ANDROID_TOOLCHAIN_PREFIX}strip libphysics.a
        )
endif()

install (TARGETS physics
         ARCHIVE DESTINATION .)

 if(NOT ${CMAKE_GENERATOR} MATCHES Xcode
        AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten"
        AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_executable(test "test/main.c")
        add_dependencies(test kernel math m physics)
        target_link_libraries(test kernel math m physics)
endif()
