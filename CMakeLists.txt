cmake_minimum_required(VERSION 4.0)
project(wlevoi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(unofficial-brotli CONFIG REQUIRED)

add_executable(wlevoi main.cpp
        wlevoi/helpers/extract.h
        wlevoi/helpers/pe.h
        wlevoi/helpers/information.h
        wlevoi/helpers/con.h
        wlevoi/helpers/includes.h
        wlevoi/helpers/offsets.h
        wlevoi/helpers/compression.h)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")

target_link_libraries(wlevoi PRIVATE
        unofficial::brotli::brotlidec
)

if(WIN32)
    add_custom_command(TARGET wlevoi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Users/voltas/vcpkg/installed/x64-windows/bin/brotlidec.dll"
            "C:/Users/voltas/vcpkg/installed/x64-windows/bin/brotlicommon.dll"
            $<TARGET_FILE_DIR:wlevoi>
    )
endif()