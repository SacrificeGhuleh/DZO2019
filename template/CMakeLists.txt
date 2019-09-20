CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(TEMPLATE)

# add libs you need
SET(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)
SET(SRC main.cpp)
SET(PCH pch.h)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC} ${PCH})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS})