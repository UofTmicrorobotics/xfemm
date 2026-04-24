cmake_minimum_required(VERSION 3.11)
project(cfemm)

include(FetchContent)

# version number information
function(setVersion prefix major minor)
	set(${prefix}_VERSION_MAJOR "${major}" PARENT_SCOPE)
	set(${prefix}_VERSION_MINOR "${minor}" PARENT_SCOPE)
	if("${ARGC}" GREATER 3)
		set(patch "${ARGV3}")
	else()
		set(patch "0")
	endif()
	if("${ARGC}" GREATER 4)
		set(extra "-${ARGV4}")
	endif()
	set(${prefix}_VERSION_PATCH "${patch}" PARENT_SCOPE)
	set(${prefix}_VERSION_STRING "${major}.${minor}.${patch}${extra}" PARENT_SCOPE)
endfunction(setVersion)

setVersion(XFEMM 0 0 0 dev)


enable_testing()
set(CMAKE_CXX_STANDARD 14)

set(GCC_COVERAGE_COMPILE_FLAGS)
set(GCC_WARNING_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CMAKE_CXX_FLAGS} ${GCC_WARNING_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} ${EXTRA_CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}")


# # Dependencies
# FetchContent_Declare(
#   eigen
#   URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
# )

# set(FETCHCONTENT_QUIET OFF)
# FetchContent_MakeAvailable(eigen)


add_subdirectory(libfemm)
add_subdirectory(fmesher)
add_subdirectory(fsolver)
# add_subdirectory(fpproc)
