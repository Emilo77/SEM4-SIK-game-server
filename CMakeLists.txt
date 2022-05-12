cmake_minimum_required(VERSION 3.22)
project(Zadanie02)

set(CMAKE_CXX_STANDARD 20)

add_executable(Zadanie02
        Utils.h
        Buffer.cpp
        Game.cpp
        ServerParameters.cpp Server.cpp)