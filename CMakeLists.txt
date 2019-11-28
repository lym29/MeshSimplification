cmake_minimum_required(VERSION 3.10.0)
project(MeshSimplification)
################### Variables. ####################
# Change if you want to modify path or other values. #
###################################################
set(PROJECT_NAME MeshSimplification)
# Output Variables
set(OUTPUT_DEBUG /Debug/)
set(OUTPUT_RELEASE /Release/)
# Folders files
set(PROJECT_DIR ./MeshSimpAlg/)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} C:/OpenMesh-8.0/OpenMesh-8.0/cmake/)
set(FreeGLUT_DIR "./freegl/lib/cmake/FreeGLUT/")
set(FreeGLUT_INCLUDE_DIR "./freegl/include/")
message(STATUS ${CMAKE_MODULE_PATH})

###############################
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
if(EIGEN3_FOUND)
	include_directories(${EIGEN3_INCLUDE_DIR})
	message(STATUS ${EIGEN3_INCLUDE_DIR})
else()
	message(STATUS "NOTICE: Eigen library is not found.") 
endif()

find_package(FreeGLUT REQUIRED)
if(FreeGLUT_FOUND)
	include_directories(${FreeGLUT_INCLUDE_DIR})
	message(STATUS ${FreeGLUT_INCLUDE_DIR})
else()
	message(STATUS "NOTICE: FreeGLUT library is not found.") 
endif()

find_package(OpenMesh REQUIRED MODULE)
message(STATUS ${OPENMESH_INCLUDE_DIRS})
if(OPENMESH_FOUND)
	include_directories(${OPENMESH_INCLUDE_DIRS})
	message(STATUS ${OPENMESH_LIBRARIES})
else()
	message(FATAL_ERROR "NOTICE: OPENMESH library is not found.") 
endif()

############## CMake Project ################
#        The main options of project        #
#############################################

#project(${PROJECT_NAME} CXX)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

# Definition of Macros
add_definitions(
   -D_USE_MATH_DEFINES
   -DFREEGLUT_STATIC
   -D_LIB
   -DFREEGLUT_LIB_PRAGMAS=0
)

############## Artefacts Output #################
# Defines outputs , depending Debug or Release. #
#################################################

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_DEBUG}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_DEBUG}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_DEBUG}")
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_RELEASE}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_RELEASE}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_RELEASE}")
endif()

################# Flags ################
# Defines Flags for Windows and Linux. #
########################################

if(MSVC)
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /MD /Od /EHsc")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /GL /Od /Oi /Gy /EHsc")
endif(MSVC)
if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   endif()
endif(NOT MSVC)

################ Files ################
#   --   Add files to project.   --   #
#######################################

file(GLOB SRC_FILES
    ${PROJECT_DIR}/*.cpp
)
include_directories(${FREEGL_DIR}include/)
# Add executable to build.
add_executable(${PROJECT_NAME} 
   ${SRC_FILES}
)

find_library(FreeGLUT_LIBRARIES
    NAMES freeglut freeglutd
    HINTS "./freegl/lib/"
)

if(MSVC)
   target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENMESH_LIBRARIES} ${FreeGLUT_LIBRARIES} opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib )
else()
   target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENMESH_LIBRARIES} ${FreeGLUT_LIBRARIES})
endif(MSVC)