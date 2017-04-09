### YOU SHOULD EDIT THE BELOW WITH YOUR DETAILS AND FILES ###

set(user1 benohayon)
set(user2 uribracha)
set(user3 noamgottlieb)
set(id1 "304848518")
set(id2 "313296550")
set(id3 "201606951")

cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)

set (SOURCE_FILES
	hw1/IBattleshipGameAlgo.h
	hw1/Player.h
	hw1/Ship.h
	hw1/Player.cpp
	hw1/Ship.cpp
	hw1/main.cpp
)
add_executable(hw1 ${SOURCE_FILES})

### DO NOT CHANGE FROM HERE ON ###

message("user: ${user1}, id: ${id1}")
message("user: ${user2}, id: ${id2}")
message("user: ${user3}, id: ${id3}")

set(EXE_NAME "ex1.${user1}")
add_executable( ${EXE_NAME}
		${SOURCE_FILES}
		)
#BUILD_BINARY_OUTPUT_DIRECTORY
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "ex1/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")