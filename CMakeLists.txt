cmake_minimum_required(VERSION 3.2)
PROJECT(opencvtest)

find_package(OpenCV 3.2.0 REQUIRED)

INCLUDE_DIRECTORIES(
  include
)

ADD_EXECUTABLE(${PROJECT_NAME} opencvtest.cpp function.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS})
