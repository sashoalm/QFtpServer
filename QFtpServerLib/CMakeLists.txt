# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(QFtpServerLib LANGUAGES CXX)

list(APPEND QT_COMPONENTS Network)
set(QT_LIBRARIES)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
message("${PROJECT_NAME} QT_LIBRARIES:${QT_LIBRARIES}")   

set(INSTALL_HEADER_FILES
    ftpserver.h
    qftpserverlib_global.h
    )

set(HEADER_FILES
    ${INSTALL_HEADER_FILES}
    dataconnection.h
    ftpcommand.h
    ftpcontrolconnection.h
    ftplistcommand.h
    ftpretrcommand.h
    ftpstorcommand.h
    sslserver.h
    )

set(SOURCE_FILES
    dataconnection.cpp
    ftpcommand.cpp
    ftpcontrolconnection.cpp
    ftplistcommand.cpp
    ftpretrcommand.cpp
    ftpserver.cpp
    ftpstorcommand.cpp
    sslserver.cpp
    )

add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${QT_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE QFTPSERVERLIB_LIBRARY)
target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${INSTALL_HEADER_FILES}" # Install head files
    VERSION ${BUILD_VERSION}
    )

export(TARGETS ${PROJECT_NAME}
    APPEND FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    )
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    )
install(EXPORT ${PROJECT_NAME}Targets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
CONFIGURE_FILE(
    "${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake.in"
    IMMEDIATE @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake.in"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    RENAME ${PROJECT_NAME}Config.cmake)

write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${BUILD_VERSION}
    COMPATIBILITY AnyNewerVersion)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
