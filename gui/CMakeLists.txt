cmake_minimum_required(VERSION 2.8.9)
project (gui C)

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

add_library(opengl STATIC IMPORTED)
set_target_properties(
        opengl
        PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_INPUT}/libopengl.a"
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

add_library(cups STATIC IMPORTED)
set_target_properties(
        cups
        PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_INPUT}/libcups.a"
)

add_library(freetype STATIC IMPORTED)
set_target_properties(
        freetype
        PROPERTIES
        IMPORTED_LOCATION "${LIBRARY_INPUT}/libfreetype.a"
)

include_directories(include)
include_directories(../math/include)
include_directories(../kernel/include)
include_directories(../png/include)
include_directories(../jpeg/include)
include_directories(../zip/include)
include_directories(../opengl/include)
include_directories(../cups/include)
include_directories(../freetype/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
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

add_library(gui STATIC ${SOURCES})
target_link_libraries(gui jpeg png zip opengl kernel math m ${OPENGL_LINK})

if(${CMAKE_BUILD_TYPE} MATCHES "Release" AND ${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_custom_command(
                TARGET gui
                POST_BUILD
                COMMAND ${ANDROID_TOOLCHAIN_PREFIX}strip libgui.a
        )
endif()

install (TARGETS gui
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
        add_dependencies(test freetype cups gui opengl kernel pthread zip jpeg png ${OPENGL_LINK})
        target_link_libraries(test freetype cups gui opengl kernel pthread zip jpeg png ${OPENGL_LINK})
 endif()
