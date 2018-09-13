cmake_minimum_required(VERSION 2.8.9)
project (jpeg C)

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
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-unused-command-line-argument")
                set(LIBRARY_INPUT "${CMAKE_INSTALL_PREFIX}")
        endif()
endif()

include_directories(include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(GLOB_RECURSE SOURCES "src/*.c" "src/*.m")
else()
    file(GLOB_RECURSE SOURCES "src/*.c")
endif()

add_library(jpeg STATIC ${SOURCES})
target_link_libraries(jpeg z)

if(${CMAKE_BUILD_TYPE} MATCHES "Release" AND ${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_custom_command(
                TARGET jpeg
                POST_BUILD
                COMMAND ${ANDROID_TOOLCHAIN_PREFIX}strip libjpeg.a
        )
endif()

install (TARGETS jpeg
         ARCHIVE DESTINATION .)
