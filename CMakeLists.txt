cmake_minimum_required(VERSION 3.16)
project(Zadanie02_server)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS " -pthread -Wall -Wextra -Wconversion -Werror -O2 -std=gnu++17")


SET(Boost_USE_STATIC_LIBS FALSE)
FIND_PACKAGE(Boost REQUIRED COMPONENTS system program_options)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF (Boost_FOUND)

SET(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_executable(robots-server
        Buffer.cpp
        ServerParametersParser.cpp
        Server.cpp
        Connection.cpp
        Game.cpp
        GameRoom.cpp
        ServerParameters.h)

TARGET_LINK_LIBRARIES(robots-server ${USED_LIBS})