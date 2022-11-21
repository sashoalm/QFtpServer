# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 3.5)

project(QFtpServer)

SET(QT_COMPONENTS Gui Widgets)
if(ANDROID)
    SET(QT_COMPONENTS ${QT_COMPONENTS} AndroidExtras)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
message("${PROJECT_NAME} QT_LIBRARIES:${QT_LIBRARIES}")    

set(HEADER_FILES
    mainwindow.h
    ftpthread.h
    debuglogdialog.h
    )

set(SOURCE_FILES
    main.cpp
    mainwindow.cpp
    debuglogdialog.cpp
    ftpthread.cpp
    )

set(UI_FILES
    mainwindow.ui
    debuglogdialog.ui
    )

set(RCC_FILES
    res.qrc)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${UI_FILES} ${RCC_FILES})
target_link_libraries(${PROJECT_NAME} QFtpServerLib ${QT_LIBRARIES})
install(TARGETS ${PROJECT_NAME})
