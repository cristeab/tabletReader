project(TabletReader)

cmake_minimum_required(VERSION 2.8.11)

option(QT5 "Compile using Qt5 framework" on)

if (QT5)

if (NOT CMAKE_PREFIX_PATH)
  set(CMAKE_PREFIX_PATH "/home/bogdan/Build/qt5/qtbase")
endif()
set(CMAKE_AUTOMOC on)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)
#set manually the path to poppler library
if (WIN32)
  set(POPPLER_INSTALL_PREFIX "c:/poppler")
  set(POPPLER_QT5_INCLUDE_DIR "${POPPLER_INSTALL_PREFIX}/include/poppler/qt5")
  set(POPPLER_QT5_LIBRARY "${POPPLER_INSTALL_PREFIX}/lib/poppler-qt5.lib")
elseif(APPLE)
  set(POPPLER_INSTALL_PREFIX "/usr/local")
  set(POPPLER_QT5_INCLUDE_DIR "${POPPLER_INSTALL_PREFIX}/include/poppler/qt5")
  set(POPPLER_QT5_LIBRARY "${POPPLER_INSTALL_PREFIX}/lib/libpoppler-qt5.dylib")
endif()

else(QT5)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH})
find_package(Qt4 4.7.0 REQUIRED)
find_package(KDE4 4.7.0 REQUIRED)
find_package(Okular REQUIRED)
find_package(QtMobility 1.2.0)
#work around for mer build
if (NOT QT_QTWEBKIT_INCLUDE_DIR)
  set (QT_QTWEBKIT_INCLUDE_DIR "/usr/include")
endif()

if (QT_MOBILITY_FOUND)
  set(QT_MOBILITY_INCLUDE_DIR ${QT_MOBILITY_INCLUDE_DIR}/QtMobility)
else()
  message(WARNING "Cannot find QtMobility.")
  set(NO_QTMOBILITY true)
  set(QT_MOBILITY_INCLUDE_DIR "")
endif()

endif(QT5)

if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
elseif (${CMAKE_BUILD_TYPE} MATCHES "^[Dd]ebug")
  add_definitions(-DQT_DEBUG_ENABLE_LOG)
endif()

#generate an automatic build/release number if available
#release numbers are generated from latest tag of the form [a-zA-Z_.]M.m
option(TIMESTAMP "Append timestamp to the version number" off)
option(AUTO_VERSION "Use git if available to generate a version number" on)
if(AUTO_VERSION)
  if( EXISTS "${CMAKE_SOURCE_DIR}/.git" )
    find_package(Git)
    if(GIT_FOUND)
      execute_process(COMMAND ${GIT_EXECUTABLE} describe --abbrev=4 HEAD
        OUTPUT_VARIABLE TR_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      #replace dashes with points
      STRING(REGEX REPLACE "-" "." TR_VERSION ${TR_VERSION})
      #remove leading alphanumeric characters
      string(REGEX REPLACE "^[a-zA-Z_.]+(.*)" "\\1" TR_VERSION ${TR_VERSION})
      if(TIMESTAMP)
        #get commit date
        execute_process(COMMAND ${GIT_EXECUTABLE} show -s --format=\"%ci\" HEAD
          OUTPUT_VARIABLE TR_DATE
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        STRING(REGEX REPLACE "\"" "" TR_DATE ${TR_DATE})
        set(TR_VERSION "${TR_VERSION} @ ${TR_DATE}")
      endif()
    endif()
  endif()
endif()
if(NOT TR_VERSION)
  set(TR_VERSION "3.1.0")
  message(WARNING "Cannot generate automatic build/release number. Using hard coded value.")
endif()

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/config.h.in ${CMAKE_BINARY_DIR}/src/config.h)

add_subdirectory(src)
add_subdirectory(help)

message (STATUS "Version: ${TR_VERSION}")
