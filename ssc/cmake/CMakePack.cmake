# Install the dependencies of the program
IF(MSYS)
    GET_FILENAME_COMPONENT(MINGW_BIN_PATH ${CMAKE_CXX_COMPILER} PATH)
    MESSAGE(STATUS "MINGW_BIN_PATH = ${MINGW_BIN_PATH}")
    LIST(APPEND MINGW_ADDITIONAL_LIB "${MINGW_BIN_PATH}/libgcc_s_dw2-1.dll")
    LIST(APPEND MINGW_ADDITIONAL_LIB "${MINGW_BIN_PATH}/libstdc++-6.dll")
    LIST(APPEND MINGW_ADDITIONAL_LIB "${MINGW_BIN_PATH}/libgfortran-3.dll")
    #LIST(APPEND MINGW_ADDITIONAL_LIB "${MINGW_BIN_PATH}/libquadmath-0.dll")
    FOREACH(f ${MINGW_ADDITIONAL_LIB})
        IF(EXISTS ${f})
            INSTALL(FILES ${f} DESTINATION ${LIBRARY_OUTPUT_DIRECTORY})
        ENDIF()
    ENDFOREACH()
ENDIF()

SET(PACKAGE_NAME ${PROJECT_NAME}-${${PROJECT_NAME}_VERSION})

IF (NOT WIN32) # Otherwise installs headers in /usr instead of /usr/local under Debian
    set(CPACK_SET_DESTDIR true)
ENDIF()

#####################################"
# Instructions to build an installer
IF(UNIX OR MSYS)
    INCLUDE(InstallRequiredSystemLibraries)
    SET(CPACK_PACKAGE_NAME ${PACKAGE_NAME})
    SET(CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION OFF)
    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Sirehna's Scientific Computing Framework")
    SET(CPACK_PACKAGE_VENDOR "DCNS Research")
    SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/read_me.txt")
    SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/Copyright.txt")
    SET(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_MAJOR_VERSION})
    SET(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_MINOR_VERSION})
    SET(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_PATCH_VERSION})
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${PACKAGE_NAME})
    SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
    SET(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
    SET(CPACK_PACKAGE_FILE_NAME "${PACKAGE_NAME}")
    IF(WIN32)
        # There is a bug in NSIS that does not handle full unix paths properly. Make
        # sure there is at least one set of four (4) backlasshes.
        SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\ssc.bmp")
        SET(CPACK_NSIS_INSTALLED_ICON_NAME "${PACKAGE_NAME}-setup")
        SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
        SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.sirehna.com")
        SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.sirehna.com")
        SET(CPACK_NSIS_CONTACT "charles-edouard.cady@sirehna.com")
        SET(CPACK_NSIS_MODIFY_PATH ON)
        #SET(CPACK_PACKAGE_EXECUTABLES "" "")
    ELSE(WIN32)
        SET(CPACK_STRIP_FILES "./${PACKAGE_NAME}-setup")
        SET(CPACK_SOURCE_STRIP_FILES "")
        SET(CPACK_PACKAGE_EXECUTABLES "" "")
        SET(CPACK_GENERATOR "DEB")
        SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Charles-Edouard Cady")
    ENDIF(WIN32)
    INCLUDE(CPack)
ENDIF(UNIX OR MSYS)

set(CPACK_COMPONENTS_ALL ${ALL_SSC_COMPONENTS})
install(EXPORT ${PROJECT_NAME}Targets
  FILE
    ${PROJECT_NAME}Targets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION
    ${ConfigPackageLocation}
)