cmake_minimum_required(VERSION 3.10)
project(RtspToWeb)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/inet)
add_subdirectory(thirdparty/FFmpeg)

set (SOURCE_DIR 	    "${CMAKE_SOURCE_DIR}/src")
set (INC_DIR 	        "${CMAKE_SOURCE_DIR}/inc")
set (JSON_INC           "${CMAKE_SOURCE_DIR}/thirdparty/json/include")
set (INET_INC           "${CMAKE_SOURCE_DIR}/thirdparty/inet/inc")
set (FFMPEG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/FFmpeg/include")
set (FFMPEG_LIB_DIR     "${CMAKE_SOURCE_DIR}/thirdparty/FFmpeg/lib")

message(STATUS "---")

link_directories(
    ${FFMPEG_LIB_DIR}
)

include_directories(
    ${INC_DIR}
    ${DATACHANNEL_INC} 
    ${JSON_INC} 
    ${INET_INC}/http
    ${INET_INC}/log
    ${INET_INC}/net
    ${INET_INC}/websocket
    ${INET_INC}/util
    ${FFMPEG_INCLUDE_DIR}
)

file(GLOB_RECURSE SRC_FILES
    "${SOURCE_DIR}/*.c*"
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${LIVE555_FILES} ${LIVE555_INC_FILES})

# Add the target includes for MY_PROJECT 
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})


list(APPEND EXTRA_LIBS
    nlohmann_json inet pthread avformat avutil avcodec
)

target_link_libraries ( ${PROJECT_NAME}
	${EXTRA_LIBS}
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs DESTINATION ${CMAKE_BINARY_DIR})