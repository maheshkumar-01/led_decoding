cmake_minimum_required(VERSION 2.8)
project( DetectPattern )
# Might be required to set the opencv dir path
#SET(OpenCV_DIR /Path_to_OpenCV/OpenCV-3.4.4/share/OpenCV)
find_package( OpenCV REQUIRED )
set(${PROJECT_NAME}_SRC ledpattern_detect.cpp test_mock.cpp)
set(${PROJECT_NAME}_HDR ledpattern_detect.hpp)
add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_SRC} ${${PROJECT_NAME}_HDR} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
