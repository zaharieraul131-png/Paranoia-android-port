cmake_minimum_required(VERSION 3.10)
project(paranoia_xash3d C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/paranoia_toolkit-master/code_src")

add_compile_options(
    -fcommon
    -Wno-writable-strings
    -Wno-write-strings
    -Wno-c++11-narrowing
    -Wno-pointer-to-int-cast
    -Wno-int-to-pointer-cast
    -Wno-format-security
    -fpermissive
    -include "${CMAKE_SOURCE_DIR}/android_gl/port_fixes.h"
)

add_definitions(
    -DCLIENT_WEAPONS
    -Dstricmp=strcasecmp
    -D_strnicmp=strncasecmp
    -Dstrnicmp=strncasecmp
    -D_snprintf=snprintf
    -D_vsnprintf=vsnprintf
    -DNO_VOICEGAMEMGR
)

# --- 1. CLIENT LIBRARY (client.so) ---
file(GLOB CLIENT_SOURCES
    "${SRC_DIR}/cl_dll/*.cpp"
    "${SRC_DIR}/cl_dll/*.c"
    "${SRC_DIR}/cl_dll/hl/*.cpp"
    "${SRC_DIR}/pm_shared/*.c"
    "${SRC_DIR}/game_shared/*.cpp"
)

list(FILTER CLIENT_SOURCES EXCLUDE REGEX ".*(_Sample|inputw32|soundsystem|overview)\\.cpp$")

add_library(client SHARED ${CLIENT_SOURCES})

target_include_directories(client PRIVATE
    "${CMAKE_SOURCE_DIR}/android_gl"
    "${SRC_DIR}/cl_dll"
    "${SRC_DIR}/cl_dll/hl"
    "${SRC_DIR}/common"
    "${SRC_DIR}/engine"
    "${SRC_DIR}/dlls"
    "${SRC_DIR}/pm_shared"
    "${SRC_DIR}/game_shared"
    "${SRC_DIR}/utils/vgui/include"
)

target_compile_definitions(client PRIVATE CLIENT_DLL)
target_link_options(client PRIVATE -Wl,--allow-multiple-definition -Wl,--unresolved-symbols=ignore-all)
target_link_libraries(client log m)
set_target_properties(client PROPERTIES PREFIX "")

# --- 2. SERVER LIBRARY (hl.so) ---
file(GLOB SERVER_SOURCES
    "${SRC_DIR}/dlls/*.cpp"
    "${SRC_DIR}/dlls/*.c"
    "${SRC_DIR}/pm_shared/*.c"
    "${SRC_DIR}/game_shared/*.cpp"
)

list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*(win32|shotgun)\\.cpp$")

add_library(hl SHARED ${SERVER_SOURCES})

target_include_directories(hl PRIVATE
    "${CMAKE_SOURCE_DIR}/android_gl"
    "${SRC_DIR}/dlls"
    "${SRC_DIR}/common"
    "${SRC_DIR}/engine"
    "${SRC_DIR}/pm_shared"
    "${SRC_DIR}/game_shared"
    "${SRC_DIR}/utils/vgui/include"
)

target_compile_definitions(hl PRIVATE VALVE_DLL)
target_link_options(hl PRIVATE -Wl,--allow-multiple-definition -Wl,--unresolved-symbols=ignore-all)
target_link_libraries(hl log m)
set_target_properties(hl PROPERTIES PREFIX "")
