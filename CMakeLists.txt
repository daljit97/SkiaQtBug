cmake_minimum_required(VERSION 3.14)

project(TestSkiaQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SKIA_BUILD_DIR "${CMAKE_SOURCE_DIR}/Skia")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)
find_library(SKIA NAMES skia libskia skia.dll HINTS ${SKIA_BUILD_DIR})

set(PROJECT_SOURCES
        main.cpp
        skiaitem.cpp
        qml.qrc
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TestSkiaQt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET TestSkiaQt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(TestSkiaQt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(TestSkiaQt
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(TestSkiaQt
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
  SK_GL)
target_link_libraries(TestSkiaQt
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick
  ${SKIA})

if(NOT SKIA)
    message("ERROR: SKIA NOT FOUND")
else()
    target_include_directories(TestSkiaQt PRIVATE
        ${SKIA_BUILD_DIR}
        ${SKIA_BUILD_DIR}/include)
endif()


set_target_properties(TestSkiaQt PROPERTIES WIN32_EXECUTABLE TRUE)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(TestSkiaQt)
    qt_finalize_executable(TestSkiaQt)
endif()
