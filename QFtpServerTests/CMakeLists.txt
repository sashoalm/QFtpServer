# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 3.5)

project(tst_qftpservertests)

SET(QT_COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
message("QT_LIBRARIES:${QT_LIBRARIES}")

set(SOURCE_FILES
    tst_qftpservertests.cpp 
    )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} QFtpServerLib ${QT_LIBRARIES})
