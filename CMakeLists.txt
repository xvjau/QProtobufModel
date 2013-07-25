project(ProtobufModel)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(ProtobufModel_SRCS ProtobufModel.cpp main.cpp)
qt4_automoc(${ProtobufModel_SRCS})
add_executable(ProtobufModel ${ProtobufModel_SRCS})
target_link_libraries(ProtobufModel ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS ProtobufModel RUNTIME DESTINATION bin)
