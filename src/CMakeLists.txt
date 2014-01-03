file(GLOB HDRS *.h)
file(GLOB SRCS *.cpp)

if(QT5)

if (NOT WIN32)
  #don't generate translations on Windows (use the existing ones)
  find_package(Qt5LinguistTools REQUIRED)
  file(GLOB QML qml/qt5_*.qml)
  set(FILES_TO_TRANSLATE ${HDRS} ${SRCS} ${QML})
  set(TRANSLATION_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Qt5_TabletReader_fr.ts 
    ${CMAKE_CURRENT_SOURCE_DIR}/Qt5_TabletReader_ro.ts)
  qt5_create_translation(QMS ${FILES_TO_TRANSLATE} ${TRANSLATION_FILES})
endif()

add_definitions(-DNO_QTMOBILITY -DPOPPLER_BACKEND -DQT5)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${POPPLER_QT5_INCLUDE_DIR})
qt5_add_resources(RSCS resources.qrc)
add_executable (${CMAKE_PROJECT_NAME} ${SRCS} ${RSCS} ${QMS})
qt5_use_modules (${CMAKE_PROJECT_NAME} Core Widgets Quick)
target_link_libraries(${CMAKE_PROJECT_NAME} ${POPPLER_QT5_LIBRARY})

else(QT5)

QT4_ADD_RESOURCES(RSCS images.qrc)
QT4_WRAP_CPP(MOCS okulardocument.h pageprovider.h window.h filebrowsermodel.h qmlcppmediator.h)

if (NOT WIN32)
  #don't generate translations on Windows (use the existing ones)
  set(QML qml/main.qml qml/zoompage.qml qml/ModalDialog/ModalDialog.qml qml/gotopage.qml
    qml/Core/calculator.js qml/maintoolbar.qml)
  set(FILES_TO_TRANSLATE ${HDRS} ${SRCS} ${QML})
  set(TRANSLATION_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}_fr.ts 
    ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}_ro.ts)
  QT4_CREATE_TRANSLATION(QMS ${FILES_TO_TRANSLATE} ${TRANSLATION_FILES})
endif()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDECLARATIVE_INCLUDE_DIR}
	${OKULAR_INCLUDE_DIR} ${OKULAR_INCLUDE_DIR}/okular)
add_executable(${CMAKE_PROJECT_NAME} ${SRCS} ${RSCS} ${MOCS} ${QMS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
  ${QT_QTDECLARATIVE_LIBRARY} ${QT_MOBILITY_SYSTEMINFO_LIBRARY} ${OKULAR_CORE_LIBRARY} ${KDE4_KDECORE_LIBRARY})
endif(QT5)

if (WIN32)
  set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${QT_QTMAIN_LIBRARY})
endif()

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${CMAKE_SOURCE_DIR}/tabletReader.desktop DESTINATION share/applications/kde4)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/hicolor DESTINATION share/icons)

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/TabletReader_ro.qm)
  configure_file(${CMAKE_CURRENT_BINARY_DIR}/TabletReader_ro.qm 
    ${CMAKE_CURRENT_SOURCE_DIR}/TabletReader_ro.qm COPYONLY)
endif()
if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/TabletReader_fr.qm)
  configure_file(${CMAKE_CURRENT_BINARY_DIR}/TabletReader_fr.qm 
    ${CMAKE_CURRENT_SOURCE_DIR}/TabletReader_fr.qm COPYONLY)
endif()

if (NOT QT5)
#setup installers
set(CPACK_PACKAGE_NAME "TabletReader")
set(CPACK_PACKAGE_VENDOR "Bogdan Cristea")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "e-book reader for touch enabled devices")
set(CPACK_PACKAGE_VERSION ${TR_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/Copyright.txt)
if (WIN32)
  if (NOT DEPS_FILE)
    set(DEPS_FILE "${CMAKE_SOURCE_DIR}/win/file_loc.txt")
  endif()
  if (NOT KDE_ROOT)
    set(KDE_ROOT "${OKULAR_INCLUDE_DIR}/../")
  endif()
  if (NOT EXISTS ${DEPS_FILE})
    message(FATAL_ERROR "Please provide the dependences file in DEPS_FILE")
  endif()
  if (NOT IS_DIRECTORY ${KDE_ROOT})
    message(FATAL_ERROR "Please provide the KDE root path in KDE_ROOT")
  endif()
  FILE(STRINGS ${DEPS_FILE} contents)
  foreach(line ${contents})
    if (${line} MATCHES "^[a-z]")
      STRING(REGEX REPLACE "\\\\" "/" dep_path ${line})
      STRING(REGEX MATCH "(.*)[/\\]" DIR ${dep_path})
      install(FILES ${KDE_ROOT}/${dep_path} DESTINATION ${DIR})
    endif()
  endforeach()
  install(FILES ${CMAKE_SOURCE_DIR}/src/icons/tabletReader.ico DESTINATION bin)
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}.exe")
  set(CPACK_NSIS_CONTACT "cristeab@gmail.com")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${PROJECT_NAME}.exe")
  set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/src/icons/tabletReader.ico")
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/src/icons/tabletReader.ico")
  set(CPACK_NSIS_URL_INFO_ABOUT "https://projects.kde.org/projects/playground/edu/ebookreader")
  set(CPACK_NSIS_CONTACT "Bogdan Cristea <cristeab\@gmail.com>")
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\bin\\\\kbuildsycoca4.exe\\\"'
  CreateShortCut \\\"$DESKTOP\\\\tabletReader.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\tabletReader.ico\\\"
  CreateShortCut \\\"$SMPROGRAMS\\\\tabletReader.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\tabletReader.ico\\\"")
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "Delete \\\"$DESKTOP\\\\tabletReader.lnk\\\"
  Delete \\\"$SMPROGRAMS\\\\tabletReader.lnk\\\"")

elseif (UNIX)
  set(CPACK_GENERATOR RPM)
endif()

include(CPack)

endif(NOT QT5)