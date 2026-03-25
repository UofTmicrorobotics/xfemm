cmake_minimum_required(VERSION 3.0.0)
project(cfemm)

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

# # set the output directories
# if(NOT CMAKE_CROSSCOMPILING)
#     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# endif()

# macro(add_flag _flag _docstring)
#     option(${_flag} ${_docstring} OFF)
#     if (${_flag})
#         message(STATUS "Enabling ${_flag}")
#         add_definitions(-D${_flag})
#     endif()
# endmacro()
# add_flag(DEBUG_FEMMLUA "Enable debug output for lua interface")
# add_flag(DEBUG_FEMMCLI "Enable debug output for femmcli")
# add_flag(DEBUG_PARSER "Enable debug output for parser functions")


add_subdirectory(libfemm)
add_subdirectory(femmcli)
add_subdirectory(fmesher)
add_subdirectory(fsolver)
add_subdirectory(fpproc)

# install(
#    FILES LICENSE-FEMM.txt LICENSE-Lua.txt LICENSE-triangle.txt
#    DESTINATION doc/xfemm
#    )
# install(
#    FILES "README-LUA.txt"
#    DESTINATION doc/xfemm
#    )


# ## installer support using cpack (this section should come last):
# set(CPACK_PACKAGE_NAME "xfemm")
# set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/../README.md")
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "xfemm - cross platform FEMM")
# set(CPACK_PACKAGE_INSTALL_DIRECTORY "xfemm")
# # maybe find a more useful vendor name?
# set(CPACK_PACKAGE_VENDOR "xfemm.sf.net")
# set(CPACK_PACKAGE_VERSION_STRING "${XFEMM_VERSION_STRING}")
# set(CPACK_PACKAGE_VERSION_MAJOR "${XFEMM_VERSION_MAJOR}")
# set(CPACK_PACKAGE_VERSION_MINOR "${XFEMM_VERSION_MINOR}")
# set(CPACK_PACKAGE_VERSION_PATCH "${XFEMM_VERSION_PATCH}")
# set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/INSTALL-MESSAGE.txt")

# # allow user to add bin dir to path:
# set(CPACK_NSIS_MODIFY_PATH "ON")
# include(CPack)
