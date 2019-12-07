CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
INCLUDE(../cmake-precompiled-header/PrecompiledHeader.cmake)
PROJECT(cv05_Fourier)

# add libs you need
SET(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)
SET(SRC main.cpp fouriertransform.cpp fouriertransform.h timer.h)

find_package(OpenMP REQUIRED)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC} pch.cpp pch.h)
#ADD_PRECOMPILED_HEADER(${PROJECT_NAME} pch.h SOURCE_CXX pch.cpp FORCEINCLUDE)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS}  OpenMP::OpenMP_CXX)