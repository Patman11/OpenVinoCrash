cmake_minimum_required(VERSION 3.22)
set (CMAKE_CXX_STANDARD 17)
set(EXE_NAME CLCrash)
project(${EXE_NAME} LANGUAGES CXX VERSION 1.0.0)
set(PROJECT_VERSION_STR ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if( WIN32 )
	set( CMAKE_SYSTEM_VERSION "10.0.18362.0" )
	set( CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.18362.0" )
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set( CUSTOM_INSTALL_PATH "${CMAKE_BINARY_DIR}/installed" )
	message("Setting default install path: ${CUSTOM_INSTALL_PATH}")
  set(CMAKE_INSTALL_PREFIX "${CUSTOM_INSTALL_PATH}" CACHE PATH "..." FORCE)
endif()

set(EXE_FILE_VERSION ${CMAKE_PROJECT_VERSION_MAJOR},${CMAKE_PROJECT_VERSION_MINOR},${CMAKE_PROJECT_VERSION_PATCH},0)
configure_file(support/version.rc.in ${CMAKE_BINARY_DIR}/version.rc)

add_executable(${EXE_NAME} WIN32)
target_sources(
	${EXE_NAME}
	PRIVATE
	src/main.cpp
	src/win.h
	src/win.cpp
	src/sample/sample.h
	src/sample/sample.cpp
	version.rc
)

find_package(OpenVINO)
add_subdirectory(src/sample/common/format_reader EXCLUDE_FROM_ALL)
add_subdirectory(src/sample/common/utils EXCLUDE_FROM_ALL)
target_link_libraries(${EXE_NAME} openvino::runtime format_reader ie_samples_utils)

include(OpenVINOBundle)

set(PACKAGE_DIR package)
set(APPX_PACKAGE_ID "ForDebug.${EXE_NAME}")
set(APPX_PUBLISHER "CN=DEBUG_APP_SIGN_ONLY")
set(APPX_VERSION ${PROJECT_VERSION_STR})
set(APPX_DISPLAY_NAME ${EXE_NAME})
set(APPX_EXECUTABLE_PATH bin/${EXE_NAME}.exe)
set(APPX_NAME ${APPX_PACKAGE_ID}.${APPX_VERSION})

configure_file(support/AppxManifest.xml.in AppxManifest.xml)

add_custom_target(
	appx DEPENDS ${EXE_NAME}
	COMMAND ${CMAKE_COMMAND} -E rm -rf appx_temp ${PACKAGE_DIR}
	COMMAND ${CMAKE_COMMAND} -E make_directory appx_temp/bin ${PACKAGE_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy AppxManifest.xml appx_temp
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${EXE_NAME}> appx_temp/bin
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:format_reader> appx_temp/bin
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openvino::runtime> appx_temp/bin
	COMMAND ${CMAKE_COMMAND} -E copy ${CLCRASH_OPENVINO_RUNTIME_INSTALL_FILES} appx_temp/bin
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/support/appx appx_temp
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/support/model appx_temp/bin/model
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/support/image appx_temp/bin/image
	COMMAND makeappx.exe pack /v /o /d appx_temp -p ${PACKAGE_DIR}/${APPX_NAME}.msix
	COMMAND signtool sign /debug /fd sha256 /a /f ${CMAKE_SOURCE_DIR}/support/sign/DEBUG_APP_SIGN_ONLY.pfx /p 1234 ${PACKAGE_DIR}/${APPX_NAME}.msix
)

set(INSTALL_DIR_NAME ${EXE_NAME}-${PROJECT_VERSION_STR})

install(
	TARGETS 
	${EXE_NAME}
	DESTINATION ${INSTALL_DIR_NAME}
)
install(
	FILES
	${CLCRASH_OPENVINO_RUNTIME_INSTALL_FILES}
	$<TARGET_FILE:format_reader>
	DESTINATION ${INSTALL_DIR_NAME}
)

install(
	DIRECTORY
	support/image
	support/model
	DESTINATION ${INSTALL_DIR_NAME}
)

set(CPACK_PACKAGE_NAME ${INSTALL_NAME_DIR})
set(CPACK_OUTPUT_FILE_PREFIX ${PACKAGE_DIR})
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP")
include(CPack)