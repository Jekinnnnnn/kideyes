add_executable (${PROJECT_NAME} "frame_util.cpp" "frame_util.hpp")

target_include_directories (
	${PROJECT_NAME}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
	${PROJECT_NAME}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/lib/libavcodec.a
	${CMAKE_CURRENT_SOURCE_DIR}/lib/libavformat.a
	${CMAKE_CURRENT_SOURCE_DIR}/lib/libavutil.a
	${CMAKE_CURRENT_SOURCE_DIR}/lib/libswresample.a
	${CMAKE_CURRENT_SOURCE_DIR}/lib/libswscale.a
)

if(MSVC)
	# Exclude libcmt when linking in visual studio
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO /NODEFAULTLIB:libcmt.lib")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" "bcrypt.lib")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4018)

	find_library(BCRYPT bcrypt)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${BCRYPT})
endif(MSVC)

