CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
INCLUDE(../cmake-precompiled-header/PrecompiledHeader.cmake)
PROJECT(cv03_Convolution)

# add libs you need
SET(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)
SET(SRC main.cpp convolutionmask.h convolution.h convolution.cpp)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC} pch.cpp)
ADD_PRECOMPILED_HEADER(${PROJECT_NAME} pch.h SOURCE_CXX pch.cpp FORCEINCLUDE)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS})