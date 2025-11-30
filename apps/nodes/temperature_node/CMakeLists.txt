# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tempreature_node)

# Add common libraries source files and public headers
get_filename_component(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../common ABSOLUTE)
zephyr_include_directories(${COMMON_DIR}/include)
FILE(GLOB common_sources ${COMMON_DIR}/*.c)

# Add app source files
FILE(GLOB app_sources src/*.c)

target_sources(app PRIVATE ${app_sources} ${common_sources})

target_include_directories(app PRIVATE ${ZEPHYR_BASE}/subsys/net/ip)