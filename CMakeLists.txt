cmake_minimum_required(VERSION 3.30)
project(ElectrostaticHalftoning)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

file(GLOB SOURCES_FILES
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/src/core/*.cpp
        ${PROJECT_SOURCE_DIR}/src/gui/*.cpp
)

file(GLOB HEADER_FILES
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/*.hpp
        ${PROJECT_SOURCE_DIR}/src/core/*.hpp
        ${PROJECT_SOURCE_DIR}/src/gui/*.hpp
)

add_executable(ElectrostaticHalftoning ${SOURCES_FILES} ${HEADER_FILES})
target_include_directories(ElectrostaticHalftoning PUBLIC ${HEADER_FILES})

target_link_libraries(ElectrostaticHalftoning Qt::Core Qt::Gui Qt::Widgets)


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=bounds")
#set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=bounds")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")