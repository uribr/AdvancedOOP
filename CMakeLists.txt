cmake_minimum_required(VERSION 3.7)
project(hw1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        hw1/IBattleshipGameAlgo.h
        hw1/Player.h
        hw1/Player.cpp
        hw1/main.cpp
        hw1/Ship.h
        hw1/Ship.cpp hw1/BonusLib.h hw1/BonusLib.cpp)
add_executable(hw1 ${SOURCE_FILES})