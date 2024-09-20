# ----------------------------------------------------------------------------
#  Root Project CMake file
# ----------------------------------------------------------------------------
#
# Example CMake build line for windows & VS2015
# windows w/cuda: cmake -G "Visual Studio 14 2015 Win64" -DUSE_AVX_INSTRUCTIONS=ON ..
# linux w/o cuda: cmake -DUSE_AVX_INSTRUCTIONS=ON -DDLIB_USE_CUDA=OFF ..
#
#
# To compile:
# windows: cmake --build . --config Release
# linux: cmake --build . --config Release -- -j10
#

cmake_minimum_required(VERSION 3.9)

#------------------------------------------------------------------------------
# Custom Build Options
#option(CROSS_COMPILE "Switch that tells CMake that we want to cross compile for an ARM based processor" OFF)
#option(BUILD_ON_PI "Switch that tells CMake to cross compile on a Raspberry PI or Linux based PC" OFF)

#------------------------------------------------------------------------------
PROJECT(mem_test_project)
set(exe_name "mem_tester")
set(default_build_type "Release")

set(CMAKE_CXX_STANDARD 17)

set(PLATFORM "$ENV{PLATFORM}" CACHE INTERNAL "Copied from environment variable")
if("${PLATFORM}" STREQUAL "")
    set(PLATFORM "NOT_SET")
    message("The PLATFORM Environment variable is not set on this system.  Setting it to:")
endif()

message("Platform: " ${PLATFORM})

# find the root of the main project
string(FIND ${PROJECT_SOURCE_DIR} "/" INDEX REVERSE)
string(SUBSTRING ${PROJECT_SOURCE_DIR} 0 ${INDEX} PR_ROOT)

#------------------------------------------------------------------------------
if(WIN32)
    if(${PLATFORM} STREQUAL "Laptop")
        set(COMMON_ROOT "D:/Common")
    elseif(${PLATFORM} STREQUAL "LaptopN")
        set(COMMON_ROOT "C:/Projects/Common")   
    elseif(${PLATFORM} STREQUAL "BEAST")
        set(COMMON_ROOT "D:/Common")
    else()
        set(COMMON_ROOT "D:/Common")
    endif()

else()

    set(HOME "$ENV{HOME}" CACHE INTERNAL "Copied from environment variable")

    if(${PLATFORM} STREQUAL "HPC")
        set(COMMON_ROOT "~/Projects/Common")
    elseif(${PLATFORM} STREQUAL "MainGear")
        set(COMMON_ROOT "~/Projects/Common")
    else()
        set(COMMON_ROOT "~/Projects/Common")
    endif()
    
    set(CMAKE_CXX_FLAGS "-O3")
    
endif()

cmake_host_system_information(RESULT HAS_SSE2 QUERY HAS_SSE2)



# grab all of the *.cpp files in the src directory
file(GLOB SOURCES "src/*.cpp" 
    )

set(project_includes "include" 
    )

set(project_libs ${project_libs}
    )

#------------------------------------------------------------------------------
message(" ")
message("--------------------------------------------------------")
message("Source Files: ")
foreach(S ${SOURCES})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------")

message(" ")

message("--------------------------------------------------------")
message("Project Includes: ")
foreach(S ${project_includes})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------")

message(" ")

message("--------------------------------------------------------------------------------")
message("Project Libraries: ")
foreach(S ${project_libs})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------------------------------")

message(" ")

#------------------------------------------------------------------------------
include_directories(${project_includes})
add_executable(${exe_name} ${SOURCES})
target_link_libraries(${exe_name} ${project_libs})

if(HAS_SSE2)
    target_compile_options(${exe_name} PRIVATE -msse2)
endif()

if(WIN32)
    #target_link_libraries(${exe_name} dlib::dlib ${OpenCV_LIBS} ${OBJ_DET_LIBS})
    message("MSVC Version: " ${MSVC_VERSION})
    
    # uncomment for MSVC and cmake greater than 3.6.0
    if(MSVC AND (${CMAKE_VERSION} VERSION_GREATER "3.6.0"))
        target_compile_options(${exe_name} PRIVATE "/MP")
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${exe_name})
    endif()
    
else()
    #target_link_libraries(${exe_name} ${OpenCV_LIBS} ${OBJ_DET_LIBS})
endif()

# ----------------------------------------------------------------------------
message(" ")
message(STATUS "Build Complete!")
message(STATUS "cmake --build . --config Release -- -j4")
message(" ")

