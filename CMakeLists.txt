set(user1 benohayon)
set(user2 uribracha)
set(user3 noamgottlieb)
set(id1 "304848518")
set(id2 "313296550")
set(id3 "201606951")
set (SOURCE_FILES
		file1.h
		file1.cpp
		file2.h
		file2.cpp
		)

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