cmake_minimum_required(VERSION 2.8.9)
project (math C)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_COMPILER "emcc")

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


include_directories(include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(GLOB_RECURSE SOURCES "src/*.c" "src/*.m")
else()
    file(GLOB_RECURSE SOURCES "src/*.c")
endif()

add_library(math STATIC ${SOURCES})
target_link_libraries(math m)

if(${CMAKE_BUILD_TYPE} MATCHES "Release" AND ${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_custom_command(
                TARGET math
                POST_BUILD
                COMMAND ${ANDROID_TOOLCHAIN_PREFIX}strip libmath.a
        )
endif()

install (TARGETS math
         ARCHIVE DESTINATION .)

 if(NOT ${CMAKE_GENERATOR} MATCHES Xcode
        AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten"
        AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")
         add_executable(test "test/main.c")
         add_dependencies(test math m)
         target_link_libraries(test math m)
 endif()
