cmake_minimum_required(VERSION 2.8.9)
project (opengl C)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_COMPILER "emcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_ZLIB=1 -s USE_WEBGL2=1")
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

add_library(png STATIC IMPORTED)
set_target_properties(
        png
        PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_INPUT}/libpng.a"
)

add_library(jpeg STATIC IMPORTED)
set_target_properties(
        jpeg
        PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_INPUT}/libjpeg.a"
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
include_directories(../png/include)
include_directories(../jpeg/include)
include_directories(../zip/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
        # find_path(GLES3_INCLUDE_DIR GLES3/gl3.h
        #         HINTS ${ANDROID_NDK})
        # find_library(GLES3_LIBRARY libGLESv3.so
        #         HINTS ${GLES3_INCLUDE_DIR}/../lib)
        # find_path(EGL_INCLUDE_DIR EGL/egl.h
        #         HINTS ${ANDROID_NDK})
        # find_library(EGL_LIBRARY libEGL.so
        #         HINTS ${EGL_INCLUDE_DIR}/../lib)
        set(OPENGL_LINK GLESv3 EGL z)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        if (${CMAKE_GENERATOR} MATCHES Xcode)
                find_package(OpenGLES REQUIRED)
                include_directories(${OPENGLES_INCLUDE_DIRS})
                set(OPENGL_LINK ${OPENGLES_LIBRARY} z)
        else()
                include_directories(/System/Library/Frameworks)
                find_package(OpenGL REQUIRED)
                include_directories(${OPENGL_INCLUDE_DIRS})
                set(OPENGL_LINK ${OPENGL_LIBRARY} z)
        endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIRS})
        set(OPENGL_LINK ${OPENGL_LIBRARY})
else()
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIRS})
        set(OPENGL_LINK ${OPENGL_LIBRARY} z)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(GLOB_RECURSE SOURCES "src/*.c" "src/*.m")
else()
    file(GLOB_RECURSE SOURCES "src/*.c")
endif()

add_library(opengl STATIC ${SOURCES})
target_link_libraries(opengl jpeg png zip kernel math m ${OPENGL_LINK})

if(${CMAKE_BUILD_TYPE} MATCHES "Release" AND ${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_custom_command(
                TARGET opengl
                POST_BUILD
                COMMAND ${ANDROID_TOOLCHAIN_PREFIX}strip libopengl.a
        )
endif()

install (TARGETS opengl
         ARCHIVE DESTINATION .)

 if(NOT ${CMAKE_GENERATOR} MATCHES Xcode
        AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")
        if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
                set(CMAKE_EXECUTABLE_SUFFIX ".html")
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_SDL=2 --use-preload-plugins --preload-file ../../../data@")
        else()
                set(OPENGL_LINK ${OPENGL_LINK} SDL2)
        endif()
        add_executable(test "test/main.c")
        add_dependencies(test opengl kernel pthread zip jpeg png ${OPENGL_LINK})
        target_link_libraries(test opengl kernel pthread zip jpeg png ${OPENGL_LINK})
 endif()
