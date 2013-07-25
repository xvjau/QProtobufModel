project(ProtobufModel)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Gui Widgets Qml Quick)
find_package(Protobuf REQUIRED)

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(-std=c++11 -Wall -Wextra)

include_directories(${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Qml_INCLUDE_DIRS} ${Qt5Quick_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(ProtobufModel_SRCS qprotobufmodel.cpp main.cpp)

qt5_add_resources(ProtobufModel_RES ProtobufModel.qrc)

PROTOBUF_GENERATE_CPP(PROTOBUF_SRCS PROTOBUF_HDRS test.proto)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}") # For Qt5

add_executable(ProtobufModel ${ProtobufModel_RES} ${ProtobufModel_SRCS} ${PROTOBUF_SRCS})

target_link_libraries(ProtobufModel ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES}  ${Qt5Qml_LIBRARIES} ${Qt5Quick_LIBRARIES} ${PROTOBUF_LIBRARY})

install(TARGETS ProtobufModel RUNTIME DESTINATION bin)
