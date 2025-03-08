cmake_minimum_required(VERSION 3.14)
project(FFmpeg)

find_package(PkgConfig REQUIRED)
pkg_check_modules(AVCODEC       REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT      REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL        REQUIRED IMPORTED_TARGET libavutil)

add_library(FFmpeg INTERFACE IMPORTED GLOBAL)
target_include_directories(FFmpeg INTERFACE
    ${AVCODEC_INCLUDE_DIRS}
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
)

target_link_options(FFmpeg INTERFACE 
    ${AVCODEC_INCLUDE_DIRS}
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
)
